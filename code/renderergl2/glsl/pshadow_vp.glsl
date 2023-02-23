uniform float4x4   u_ModelViewProjectionMatrix;

void main(
	float3 attr_Position,
	float3 attr_Normal,
	float3 out var_Position : TEXCOORD0,
	float3 out var_Normal : TEXCOORD1,
	float4 out gl_Position : POSITION
) {
	gl_Position = mul(float4(attr_Position, 1.0), u_ModelViewProjectionMatrix);

	var_Position  = attr_Position;
	var_Normal    = attr_Normal;
}
