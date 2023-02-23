//#if defined(USE_DEFORM_VERTEXES)
uniform int     u_DeformGen;
uniform float    u_DeformParams[5];
//#endif

uniform float   u_Time;
uniform float4x4    u_ModelViewProjectionMatrix;

uniform float4x4   u_ModelMatrix;

#if defined(USE_VERTEX_ANIMATION)
uniform float   u_VertexLerp;
#elif defined(USE_BONE_ANIMATION)
uniform float4x4 u_BoneMatrix[MAX_GLSL_BONES];
#endif

float3 DeformPosition(const float3 pos, const float3 normal, const float2 st)
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
		phase += dot(pos.xyz, float3(spread));
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

void main(
	float3  attr_Position,
	float3  attr_Normal,
	float4  attr_TexCoord0,
#if defined(USE_VERTEX_ANIMATION)
	float3  attr_Position2,
	float3  attr_Normal2,
#elif defined(USE_BONE_ANIMATION)
	float4 attr_BoneIndexes,
	float4 attr_BoneWeights,
#endif
	float4 out gl_Position : POSITION,
	float3 out var_Position : TEXCOORD0
) {
#if defined(USE_VERTEX_ANIMATION)
	float3 position  = lerp(attr_Position, attr_Position2, u_VertexLerp);
	float3 normal    = lerp(attr_Normal,   attr_Normal2,   u_VertexLerp);
#elif defined(USE_BONE_ANIMATION)
	float4x4 vtxMat  = u_BoneMatrix[int(attr_BoneIndexes.x)] * attr_BoneWeights.x;
	     vtxMat += u_BoneMatrix[int(attr_BoneIndexes.y)] * attr_BoneWeights.y;
	     vtxMat += u_BoneMatrix[int(attr_BoneIndexes.z)] * attr_BoneWeights.z;
	     vtxMat += u_BoneMatrix[int(attr_BoneIndexes.w)] * attr_BoneWeights.w;
	float3x3 nrmMat = float3x3(cross(vtxMat[1].xyz, vtxMat[2].xyz), cross(vtxMat[2].xyz, vtxMat[0].xyz), cross(vtxMat[0].xyz, vtxMat[1].xyz));

	float3 position  = float3(mul(float4(attr_Position, 1.0), vtxMat));
	float3 normal    = normalize(mul(attr_Normal, nrmMat));
#else
	float3 position = attr_Position;
	float3 normal   = attr_Normal;
#endif

	position = DeformPosition(position, normal, attr_TexCoord0.xy);

	gl_Position = mul(float4(position, 1.0), u_ModelViewProjectionMatrix);
	
	var_Position  = (mul(float4(position, 1.0), u_ModelMatrix)).xyz;
}
