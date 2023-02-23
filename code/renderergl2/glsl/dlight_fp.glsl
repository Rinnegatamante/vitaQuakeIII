uniform sampler2D u_DiffuseMap;

uniform int       u_AlphaTest;

void main(
	float2 var_Tex1 : TEXCOORD0,
	float4 var_Color : COLOR,
	float4 out gl_FragColor : COLOR
) {
	float4 color = tex2D(u_DiffuseMap, var_Tex1);

	float alpha = color.a * var_Color.a;
	if (u_AlphaTest == 1)
	{
		if (alpha == 0.0)
			discard;
	}
	else if (u_AlphaTest == 2)
	{
		if (alpha >= 0.5)
			discard;
	}
	else if (u_AlphaTest == 3)
	{
		if (alpha < 0.5)
			discard;
	}
	
	gl_FragColor.rgb = color.rgb * var_Color.rgb;
	gl_FragColor.a = alpha;
}
