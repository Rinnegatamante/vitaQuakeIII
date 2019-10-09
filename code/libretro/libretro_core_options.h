#ifndef LIBRETRO_CORE_OPTIONS_H__
#define LIBRETRO_CORE_OPTIONS_H__

#include <stdlib.h>
#include <string.h>

#include "../libretro-common/include/libretro.h"
#include "../libretro-common/include/retro_inline.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_ENGLISH */

/* Default language:
 * - All other languages must include the same keys and values
 * - Will be used as a fallback in the event that frontend language
 *   is not available
 * - Will be used as a fallback for any missing entries in
 *   frontend language definition */

struct retro_core_option_definition option_defs_us[] = {
	{
      "vitaquakeiii_framerate",
      "Framerate (restart)",
      "Modify framerate. Requires a restart.",
      {
         { "auto",            "Auto"},
         { "50",              "50fps"},
         { "60",              "60fps"},
         { "72",              "72fps"},
         { "75",              "75fps"},
         { "90",              "90fps"},
         { "100",              "100fps"},
         { "119",              "119fps"},
         { "120",              "120fps"},
         { "144",              "144fps"},
         { "155",              "155fps"},
         { "160",              "160fps"},
         { "165",              "165fps"},
         { "180",              "180fps"},
         { "200",              "200fps"},
         { "240",              "240fps"},
         { "244",              "244fps"},
         { NULL, NULL },
      },
      "auto"
   },
   {
      "vitaquakeiii_resolution",
      "Internal resolution (restart)",
      "Configure the resolution. Requires a restart.",
      {
         { "480x272",   NULL },
         { "640x368",   NULL },
         { "720x408",   NULL },
         { "960x544",   NULL },
		 { "1280x720",   NULL },
		 { "1920x1080",   NULL },
		 { "2560x1440",   NULL },
		 { "3840x2160",   NULL },
         { NULL, NULL },
      },
      "960x544"
   },
   {
      "vitaquakeiii_invert_y_axis",
      "Invert Y Axis",
      "Invert the gamepad right analog stick's Y axis.",
      {
         { "disabled",  "Disabled" },
         { "enabled",   "Enabled" },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "vitaquakeiii_fps",
      "Show FPS",
      "Shows framerate on screen.",
      {
         { "disabled",  "Disabled" },
         { "enabled",   "Enabled" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "vitaquakeiii_pickups",
      "2D Pickups Rendering",
      "Makes pickups (medkits, weapons, quad damage, etc.) be rendered with 2D icons.",
      {
         { "disabled",  "Disabled" },
         { "enabled",   "Enabled" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "vitaquakeiii_weapon",
      "Show Equipped Weapon",
      "Shows equipped weapon on screen.",
      {
         { "disabled",  "Disabled" },
         { "enabled",   "Enabled" },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "vitaquakeiii_shadows",
      "Shadows Quality",
      "Configure the quality of shadows rendering.",
      {
         { "disabled",  "Disabled" },
         { "low",       "Low" },
		 { "high",      "High" },
         { NULL, NULL },
      },
      "low"
   },
   {
      "vitaquakeiii_filter",
      "Textures Filter",
      "Configure the textures filter to use.",
      {
         { "disabled",  "Disabled" },
         { "linear",    "Linear" },
		 { "bilinear",  "Bilinear" },
		 { "trilinear",  "Trilinear" },
         { NULL, NULL },
      },
      "bilinear"
   },
   { NULL, NULL, NULL, {{0}}, NULL },
};

/* RETRO_LANGUAGE_JAPANESE */

/* RETRO_LANGUAGE_FRENCH */

/* RETRO_LANGUAGE_SPANISH */

/* RETRO_LANGUAGE_GERMAN */

/* RETRO_LANGUAGE_ITALIAN */
struct retro_core_option_definition option_defs_it[] = {
	{
      "vitaquakeiii_framerate",
      "Framerate (riavvio)",
      "Modifica il framerate. Richiede un riavvio.",
      {
         { "auto",            "Auto"},
         { "50",              "50fps"},
         { "60",              "60fps"},
         { "72",              "72fps"},
         { "75",              "75fps"},
         { "90",              "90fps"},
         { "100",              "100fps"},
         { "119",              "119fps"},
         { "120",              "120fps"},
         { "144",              "144fps"},
         { "155",              "155fps"},
         { "160",              "160fps"},
         { "165",              "165fps"},
         { "180",              "180fps"},
         { "200",              "200fps"},
         { "240",              "240fps"},
         { "244",              "244fps"},
         { NULL, NULL },
      },
      "auto"
   },
   {
      "vitaquakeiii_resolution",
      "Risoluzione interna (riavvio)",
      "Configura la risoluzione. Richiede un riavvio.",
      {
         { "480x272",   NULL },
         { "640x368",   NULL },
         { "720x408",   NULL },
         { "960x544",   NULL },
		 { "1280x720",   NULL },
		 { "1920x1080",   NULL },
		 { "2560x1440",   NULL },
		 { "3840x2160",   NULL },
         { NULL, NULL },
      },
      "960x544"
   },
   {
      "vitaquakeiii_invert_y_axis",
      "Inverti Asse Y",
      "Inverte l'asse Y dell'analogico destro.",
      {
         { "disabled",  "Disattivato" },
         { "enabled",   "Attivato" },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "vitaquakeiii_fps",
      "Mostra FPS",
      "Mostra il framerate su schermo.",
      {
         { "disabled",  "Disattivato" },
         { "enabled",   "Attivato" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "vitaquakeiii_pickups",
      "Rendering 2D Pickup",
      "Rende i pickup (medkit, armi, danno quadruplo, ecc.) renderizzati con icone 2D.",
      {
         { "disabled",  "Disattivato" },
         { "enabled",   "Attivato" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "vitaquakeiii_weapon",
      "Mostra Arma Equipaggiata",
      "Mostra l'arma equipaggiata su schermo.",
      {
         { "disabled",  "Disattivato" },
         { "enabled",   "Attivato" },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "vitaquakeiii_shadows",
      "Qualità Ombre",
      "Configura la qualità del rendering delle ombre.",
      {
         { "disabled",  "Disattivate" },
         { "low",       "Bassa" },
		 { "high",      "Alta" },
         { NULL, NULL },
      },
      "low"
   },
   {
      "vitaquakeiii_filter",
      "Filtro Texture",
      "Configura il filtro delle texture da utilizzare.",
      {
         { "disabled",  "Disattivato" },
         { "linear",    "Lineare" },
		 { "bilinear",  "Bilineare" },
		 { "trilinear",  "Trilineare" },
         { NULL, NULL },
      },
      "bilinear"
   },
   { NULL, NULL, NULL, {{0}}, NULL },
};

/* RETRO_LANGUAGE_DUTCH */

/* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */

/* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */

/* RETRO_LANGUAGE_RUSSIAN */

/* RETRO_LANGUAGE_KOREAN */

/* RETRO_LANGUAGE_CHINESE_TRADITIONAL */

/* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */

/* RETRO_LANGUAGE_ESPERANTO */

/* RETRO_LANGUAGE_POLISH */

/* RETRO_LANGUAGE_VIETNAMESE */

/* RETRO_LANGUAGE_ARABIC */

/* RETRO_LANGUAGE_GREEK */

/* RETRO_LANGUAGE_TURKISH */

/*
 ********************************
 * Language Mapping
 ********************************
*/

struct retro_core_option_definition *option_defs_intl[RETRO_LANGUAGE_LAST] = {
   option_defs_us, /* RETRO_LANGUAGE_ENGLISH */
   NULL,           /* RETRO_LANGUAGE_JAPANESE */
   NULL,           /* RETRO_LANGUAGE_FRENCH */
   NULL,           /* RETRO_LANGUAGE_SPANISH */
   NULL,           /* RETRO_LANGUAGE_GERMAN */
   option_defs_it, /* RETRO_LANGUAGE_ITALIAN */
   NULL,           /* RETRO_LANGUAGE_DUTCH */
   NULL,           /* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */
   NULL,           /* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */
   NULL,           /* RETRO_LANGUAGE_RUSSIAN */
   NULL,           /* RETRO_LANGUAGE_KOREAN */
   NULL,           /* RETRO_LANGUAGE_CHINESE_TRADITIONAL */
   NULL,           /* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
   NULL,           /* RETRO_LANGUAGE_ESPERANTO */
   NULL,           /* RETRO_LANGUAGE_POLISH */
   NULL,           /* RETRO_LANGUAGE_VIETNAMESE */
   NULL,           /* RETRO_LANGUAGE_ARABIC */
   NULL,           /* RETRO_LANGUAGE_GREEK */
   NULL,           /* RETRO_LANGUAGE_TURKISH */
};

/*
 ********************************
 * Functions
 ********************************
*/

/* Handles configuration/setting of core options.
 * Should only be called inside retro_set_environment().
 * > We place the function body in the header to avoid the
 *   necessity of adding more .c files (i.e. want this to
 *   be as painless as possible for core devs)
 */

static INLINE void libretro_set_core_options(retro_environment_t environ_cb)
{
   unsigned version = 0;

   if (!environ_cb)
      return;

   if (environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version) && (version == 1))
   {
      struct retro_core_options_intl core_options_intl;
      unsigned language = 0;

      core_options_intl.us    = option_defs_us;
      core_options_intl.local = NULL;

      if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
          (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH))
         core_options_intl.local = option_defs_intl[language];

      environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL, &core_options_intl);
   }
   else
   {
      size_t i;
      size_t num_options               = 0;
      struct retro_variable *variables = NULL;
      char **values_buf                = NULL;

      /* Determine number of options */
      while (true)
      {
         if (option_defs_us[num_options].key)
            num_options++;
         else
            break;
      }

      /* Allocate arrays */
      variables  = (struct retro_variable *)calloc(num_options + 1, sizeof(struct retro_variable));
      values_buf = (char **)calloc(num_options, sizeof(char *));

      if (!variables || !values_buf)
         goto error;

      /* Copy parameters from option_defs_us array */
      for (i = 0; i < num_options; i++)
      {
         const char *key                        = option_defs_us[i].key;
         const char *desc                       = option_defs_us[i].desc;
         const char *default_value              = option_defs_us[i].default_value;
         struct retro_core_option_value *values = option_defs_us[i].values;
         size_t buf_len                         = 3;
         size_t default_index                   = 0;

         values_buf[i] = NULL;

         if (desc)
         {
            size_t num_values = 0;

            /* Determine number of values */
            while (true)
            {
               if (values[num_values].value)
               {
                  /* Check if this is the default value */
                  if (default_value)
                     if (strcmp(values[num_values].value, default_value) == 0)
                        default_index = num_values;

                  buf_len += strlen(values[num_values].value);
                  num_values++;
               }
               else
                  break;
            }

            /* Build values string */
            if (num_values > 1)
            {
               size_t j;

               buf_len += num_values - 1;
               buf_len += strlen(desc);

               values_buf[i] = (char *)calloc(buf_len, sizeof(char));
               if (!values_buf[i])
                  goto error;

               strcpy(values_buf[i], desc);
               strcat(values_buf[i], "; ");

               /* Default value goes first */
               strcat(values_buf[i], values[default_index].value);

               /* Add remaining values */
               for (j = 0; j < num_values; j++)
               {
                  if (j != default_index)
                  {
                     strcat(values_buf[i], "|");
                     strcat(values_buf[i], values[j].value);
                  }
               }
            }
         }

         variables[i].key   = key;
         variables[i].value = values_buf[i];
      }
      
      /* Set variables */
      environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);

error:

      /* Clean up */
      if (values_buf)
      {
         for (i = 0; i < num_options; i++)
         {
            if (values_buf[i])
            {
               free(values_buf[i]);
               values_buf[i] = NULL;
            }
         }

         free(values_buf);
         values_buf = NULL;
      }

      if (variables)
      {
         free(variables);
         variables = NULL;
      }
   }
}

#ifdef __cplusplus
}
#endif

#endif
