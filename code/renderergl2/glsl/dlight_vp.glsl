uniform float4   u_DlightInfo;

#if defined(USE_DEFORM_VERTEXES)
uniform int    u_DeformGen;
uniform float  u_DeformParams[5];
uniform float  u_Time;
#endif

uniform float4   u_Color;
uniform float4x4   u_ModelViewProjectionMatrix;

#if defined(USE_DEFORM_VERTEXES)
float3 DeformPosition(float3 pos, float3 normal, float2 st)
{
	if (u_DeformGen == 0)
	{
		return pos;
	}

	float base =      u_DeformParams[0];
	float amplitude = u_DeformParams[1];
	float phase =     u_DeformParams[2];
	float frequency = u_DeformParams[3];
	float spread =    u_DeformParams[4];

	if (u_DeformGen == DGEN_BULGE)
	{
		phase *= st.x;
	}
	else // if (u_DeformGen <= DGEN_WAVE_INVERSE_SAWTOOTH)
	{
		phase += dot(pos.xyz, float3(spread, spread, spread));
	}

	float value = phase + (u_Time * frequency);
	float func;

	if (u_DeformGen == DGEN_WAVE_SIN)
	{
		func = sin(value * 2.0 * M_PI);
	}
	else if (u_DeformGen == DGEN_WAVE_SQUARE)
	{
		func = sign(0.5 - frac(value));
	}
	else if (u_DeformGen == DGEN_WAVE_TRIANGLE)
	{
		func = abs(frac(value + 0.75) - 0.5) * 4.0 - 1.0;
	}
	else if (u_DeformGen == DGEN_WAVE_SAWTOOTH)
	{
		func = frac(value);
	}
	else if (u_DeformGen == DGEN_WAVE_INVERSE_SAWTOOTH)
	{
		func = (1.0 - frac(value));
	}
	else // if (u_DeformGen == DGEN_BULGE)
	{
		func = sin(value);
	}

	return pos + normal * (base + func * amplitude);
}
#endif

void main(
	float3 attr_Position,
	float4 attr_TexCoord0,
	float3 attr_Normal,
	float2 out var_Tex1 : TEXCOORD0,
	float4 out var_Color : COLOR,
	float4 out gl_Position : POSITION
) {
	float3 position = attr_Position;
	float3 normal = attr_Normal;

#if defined(USE_DEFORM_VERTEXES)
	position = DeformPosition(position, normal, attr_TexCoord0.xy);
#endif

	gl_Position = mul(float4(position, 1.0), u_ModelViewProjectionMatrix);
		
	float3 dist = u_DlightInfo.xyz - position;

	var_Tex1 = dist.xy * u_DlightInfo.a + float2(0.5, 0.5);
	float dlightmod = step(0.0, dot(dist, normal));
	dlightmod *= clamp(2.0 * (1.0 - abs(dist.z) * u_DlightInfo.a), 0.0, 1.0);
	
	var_Color = u_Color * dlightmod;
}
