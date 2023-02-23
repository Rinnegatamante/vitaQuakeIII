uniform float4  u_Color;

void main(
	float var_Scale : TEXCOORD0,
	float4 out gl_FragColor : COLOR
) {
	gl_FragColor = u_Color;
	gl_FragColor.a = sqrt(clamp(var_Scale, 0.0, 1.0));
}
