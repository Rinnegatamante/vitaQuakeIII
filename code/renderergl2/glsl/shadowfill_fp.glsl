uniform float4  u_LightOrigin;
uniform float u_LightRadius;

void main(
	float3 var_Position : TEXCOORD0,
	float4 out gl_FragColor : COLOR
) {
#if defined(USE_DEPTH)
	float depth = length(u_LightOrigin.xyz - var_Position) / u_LightRadius;

	// 24 bit precision
	float3 bitSh = float3( 256 * 256,         256,           1);
	float3 bitMsk = float3(        0, 1.0 / 256.0, 1.0 / 256.0);
	
	float3 comp;
	comp = depth * bitSh;
	comp.xy = fract(comp.xy);
	comp -= comp.xxy * bitMsk;
	gl_FragColor = float4(comp, 1.0);

#else
	gl_FragColor = float4(0, 0, 0, 1);
#endif
}
