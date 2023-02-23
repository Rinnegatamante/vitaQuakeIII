uniform float4x4   u_ModelViewProjectionMatrix;

void main(
	float3 attr_Position,
	float4 attr_TexCoord0,
	float2 out var_Tex1 : TEXCOORD0,
	float4 out gl_Position : POSITION
) {
	gl_Position = mul(float4(attr_Position, 1.0), u_ModelViewProjectionMatrix);
	var_Tex1 = attr_TexCoord0.xy;
}
