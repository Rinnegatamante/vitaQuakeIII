/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <vitasdk.h>
#include <vita2d.h>
#include <curl/curl.h>
#include "minizip/unzip.h"

#define Q3A   0
#define Q3TA  1
#define Q3TAS 2
#define OA    3
#define URT4  4

#define BOOTING     0
#define DOWNLOADING 1
#define DOWNLOADED  2
#define EXTRACTING  3
#define FINISHED    4
#define MISSING     5

int state = BOOTING;

#define NET_INIT_SIZE 1*1024*1024
#define CHUNK_MAXSIZE 32*1024*1024

char * cores[] = {
	"Quake III: Arena",
	"Quake III: Team Arena", // Data files
	"Quake III: Team Arena", // Dynamic libs
	"OpenArena",
	"Urban Terror"
};

char *sizes[] = {
	"B",
	"KB",
	"MB",
	"GB"
};

uint64_t downloaded_bytes = 0;
uint64_t extracted_bytes = 0;
uint64_t chunk_size = 0;
uint64_t chunk_ptr = 0;
uint64_t total_bytes = 0xFFFFFFFF;
uint8_t chunk_idx = 0;
uint8_t wchunk_idx = 0;
uint8_t end_write = 0;

FILE *fh;

curl_off_t curl_bytes;

char *header;
unsigned int headerSize;

char *bytes_string;
unsigned int bytes_len;

static CURL *curl_handle = NULL;

int _newlib_heap_size_user = 128 * 1024 * 1024;

static uint8_t chunk_buffer[2][CHUNK_MAXSIZE];

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *stream)
{
	downloaded_bytes += size * nmemb;
	return fwrite(ptr, size, nmemb, fh);
}

static size_t header_cb(char *buffer, size_t size, size_t nitems, void *userdata)
{
	if (total_bytes == 0xFFFFFFFF) {
		char *ptr = strcasestr(buffer, "Content-Length");
		if (ptr != NULL) sscanf(ptr, "Content-Length: %llu", &total_bytes);
	}
	return nitems * size;
}

static char asyncUrl[512];
static void resumeDownload()
{
	curl_easy_reset(curl_handle);
	curl_easy_setopt(curl_handle, CURLOPT_URL, asyncUrl);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 10L);
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_cb);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, bytes_string); // Dummy
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_cb);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, bytes_string); // Dummy
	curl_easy_setopt(curl_handle, CURLOPT_RESUME_FROM, downloaded_bytes);
	curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 524288);
	struct curl_slist *headerchunk = NULL;
	headerchunk = curl_slist_append(headerchunk, "Accept: */*");
	headerchunk = curl_slist_append(headerchunk, "Content-Type: application/json");
	headerchunk = curl_slist_append(headerchunk, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
	headerchunk = curl_slist_append(headerchunk, "Content-Length: 0");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerchunk);
	curl_easy_perform(curl_handle);
}

static int downloadThread(unsigned int args, void* arg){
	curl_handle = curl_easy_init();
	fh = fopen("ux0:/data/ioq3d.zip", "wb");
	while (downloaded_bytes < total_bytes) {
		resumeDownload();
	}
	fclose(fh);
	state = DOWNLOADED;
	sceKernelExitDeleteThread(0);
	return 0;
}

float format(float len) {
	while (len > 1024) len = len / 1024.0f;
	return len;
}

uint8_t quota(uint64_t len) {
	uint8_t ret = 0;
	while (len > 1024) {
		ret++;
		len = len / 1024;
	}
	return ret;
}

void launchDownload(const char *url) {
	if (state != BOOTING) return;
	sprintf(asyncUrl, "%s", url);
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
	int ret = sceNetShowNetstat();
	SceNetInitParam initparam;
	if (ret == SCE_NET_ERROR_ENOTINIT) {
		void *net_memory = malloc(NET_INIT_SIZE);
		initparam.memory = net_memory;
		initparam.size = NET_INIT_SIZE;
		initparam.flags = 0;
		sceNetInit(&initparam);
	}
	sceNetCtlInit();
	sceHttpInit(1*1024*1024);
	SceUID thd = sceKernelCreateThread("Net Downloader Thread", &downloadThread, 0x10000100, 0x100000, 0, 0, NULL);
	sceKernelStartThread(thd, 0, NULL);
	state = DOWNLOADING;
}

static int downloader_main(unsigned int args, void* arg) {
	uLong zip_idx;
	uLong zip_total;
	uint8_t extract_started = 0;
	uint64_t curr_extracted_bytes = 0;
	SceCtrlData pad;
	uint32_t oldpad;
	vita2d_pgf *pgf;
	char read_buffer[8192], fname[512];
	unz_global_info global_info;
	unz_file_info file_info;
	unzFile *zipfile;
	FILE *f;
	
	char base_dir[256];
	sprintf(base_dir, "ux0:data/");
	
	vita2d_init();
	vita2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));

	pgf = vita2d_load_default_pgf();
	
	// Reading requested core
	uint32_t core_idx;
	f = fopen("ux0:data/ioq3.d", "r");
	if (!f) { return 0; }
	fscanf(f, "%lu", &core_idx);
	fclose(f);
	sceIoRemove("ux0:data/ioq3.d");
	
	for (;;) {
		
		// Prevent screen power-off
		sceKernelPowerTick(0);
		
		sceCtrlPeekBufferPositive(0, &pad, 1);

		if ((pad.buttons & SCE_CTRL_CROSS) && (state >= FINISHED))
			break;

		vita2d_start_drawing();
		vita2d_clear_screen();

		vita2d_pgf_draw_text(pgf, 20, 30, RGBA8(255,255,0,255), 1.0f, "vitaQuakeIII downloader");
		
		vita2d_pgf_draw_textf(pgf, 20, 70, RGBA8(255,255,255,255), 1.0f, "Missing data files for %s core", cores[core_idx]);
		
		switch (core_idx) {
		case Q3A:
			vita2d_pgf_draw_textf(pgf, 20, 120, RGBA8(0,255,0,255), 1.0f, "Found shareware data files pack for %s!", cores[core_idx]);
			launchDownload("https://rinnegatamante.it/ioq3.zip");
			break;
		case Q3TA:
			vita2d_pgf_draw_text(pgf, 20, 120, RGBA8(255,0,0,255), 1.0f, "No data files available for download.");
			state = MISSING;
			break;
		case Q3TAS:
			vita2d_pgf_draw_textf(pgf, 20, 120, RGBA8(0,255,0,255), 1.0f, "Found dynamic libraries pack for %s!", cores[core_idx]);
			launchDownload("https://rinnegatamante.it/missionpack.zip");
			break;
		case OA:
			vita2d_pgf_draw_textf(pgf, 20, 120, RGBA8(0,255,0,255), 1.0f, "Found game data files (v.0.8.8) pack for %s!", cores[core_idx]);
			launchDownload("https://rinnegatamante.it/openarena.zip");
			break;
		case URT4:
			vita2d_pgf_draw_textf(pgf, 20, 120, RGBA8(0,255,0,255), 1.0f, "Found game data files (v.4.3.4) pack for %s!", cores[core_idx]);
			launchDownload("https://rinnegatamante.it/urbanterror.zip");
			break;
		}
		
		if (state > DOWNLOADING) {
			if (state >= FINISHED) vita2d_pgf_draw_textf(pgf, 20, 400, RGBA8(255,255,255,255), 1.0f, "Press X to exit.");
			vita2d_pgf_draw_textf(pgf, 20, 200, RGBA8(0,255,0,255), 1.0f, "Pack downloaded successfully! (%.2f %s)", format(downloaded_bytes), sizes[quota(downloaded_bytes)]);
			if (state < FINISHED) {
				vita2d_pgf_draw_text(pgf, 20, 220, RGBA8(255,255,255,255), 1.0f, "Extracting pack, please wait!");
				vita2d_pgf_draw_textf(pgf, 20, 300, RGBA8(255,255,255,255), 1.0f, "File: %lu / %lu", zip_idx, zip_total);
				vita2d_pgf_draw_textf(pgf, 20, 320, RGBA8(255,255,255,255), 1.0f, "Filename: %s", fname);
				vita2d_pgf_draw_textf(pgf, 20, 340, RGBA8(255,255,255,255), 1.0f, "Filesize: (%.2f %s / %.2f %s)", format(curr_extracted_bytes), sizes[quota(curr_extracted_bytes)], format(file_info.uncompressed_size), sizes[quota(file_info.uncompressed_size)]);
				vita2d_pgf_draw_textf(pgf, 20, 360, RGBA8(255,255,255,255), 1.0f, "Total Progress: (%.2f %s / %.2f %s)", format(extracted_bytes), sizes[quota(extracted_bytes)], format(total_bytes), sizes[quota(total_bytes)]);
				if (state < EXTRACTING) {
					extracted_bytes = 0;
					total_bytes = 0;
					zipfile = unzOpen("ux0:/data/ioq3d.zip");
					unzGetGlobalInfo(zipfile, &global_info);
					zip_total = global_info.number_entry;
					unzGoToFirstFile(zipfile);
					for (zip_idx = 0; zip_idx < zip_total; ++zip_idx) {
						unzGetCurrentFileInfo(zipfile, &file_info, fname, 512, NULL, 0, NULL, 0);
						total_bytes += file_info.uncompressed_size;
						if ((zip_idx + 1) < zip_total) unzGoToNextFile(zipfile);
					}
					zip_idx = 0;
					unzGoToFirstFile(zipfile);
					state = EXTRACTING;
				} 
				if (state == EXTRACTING) {
					if (zip_idx < zip_total) {
						if (!extract_started) {
							char filename[512];
							unzGetCurrentFileInfo(zipfile, &file_info, fname, 512, NULL, 0, NULL, 0);
							sprintf(filename, "%s%s", base_dir, fname); 
							const size_t filename_length = strlen(filename);
							if (filename[filename_length - 1] == '/') sceIoMkdir(filename, 0777);
							else {
								unzOpenCurrentFile(zipfile);
								f = fopen(filename, "wb");
								extract_started = 1;
							}
						}
						if (extract_started) {
							int err = unzReadCurrentFile(zipfile, read_buffer, 8192);
							if (err > 0) {
								fwrite(read_buffer, err, 1, f);
								extracted_bytes += err;
								curr_extracted_bytes += err;
							} 
							if (curr_extracted_bytes == file_info.uncompressed_size) {
								fclose(f);
								unzCloseCurrentFile(zipfile);
								extract_started = 0;
								curr_extracted_bytes = 0;
							}
						}
						if (!extract_started) {
							if ((zip_idx + 1) < zip_total) unzGoToNextFile(zipfile);
							zip_idx++;
						}
					} else {
						unzClose(zipfile);
						sceIoRemove("ux0:/data/ioq3d.zip");
						state = FINISHED;
					}
				}
			}
			if (state > EXTRACTING) vita2d_pgf_draw_text(pgf, 20, 220, RGBA8(0,255,0,255), 1.0f, "Pack extracted succesfully!");
		} else vita2d_pgf_draw_textf(pgf, 20, 200, RGBA8(255,255,255,255), 1.0f, "Downloading pack, please wait. (%.2f %s / %.2f %s)", format(downloaded_bytes), sizes[quota(downloaded_bytes)], format(total_bytes), sizes[quota(total_bytes)]);

		vita2d_end_drawing();
		vita2d_swap_buffers();
	}

	switch (core_idx) {
	case Q3A:
		sceAppMgrLoadExec("app0:eboot.bin", NULL, NULL);
		break;
	case OA:
		sceAppMgrLoadExec("app0:openarena.bin", NULL, NULL);
		break;
	case URT4:
		sceAppMgrLoadExec("app0:urbanterror.bin", NULL, NULL);
		break;
	default:
		break;
	}
	
	vita2d_free_pgf(pgf);
	vita2d_fini();
	
	return 0;
}

int main() {
	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	SceUID main_thread = sceKernelCreateThread("main_downloader", downloader_main, 0x40, 0x1000000, 0, 0, NULL);
	if (main_thread >= 0){
		sceKernelStartThread(main_thread, 0, NULL);
		sceKernelWaitThreadEnd(main_thread, NULL, NULL);
	}
	return 0;
}
