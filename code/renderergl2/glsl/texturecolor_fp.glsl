uniform sampler2D u_DiffuseMap;
uniform float4      u_Color;

float4 main(
	float2 var_Tex1 : TEXCOORD0
) {
	return tex2D(u_DiffuseMap, var_Tex1) * u_Color;
}
