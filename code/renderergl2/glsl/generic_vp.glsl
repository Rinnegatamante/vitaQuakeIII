
uniform float4   u_DiffuseTexMatrix;
uniform float4   u_DiffuseTexOffTurb;

#if defined(USE_TCGEN) || defined(USE_RGBAGEN)
uniform float3   u_LocalViewOrigin;
#endif

#if defined(USE_TCGEN)
uniform int    u_TCGen0;
uniform float3   u_TCGen0Vector0;
uniform float3   u_TCGen0Vector1;
#endif

#if defined(USE_FOG)
uniform float4   u_FogDistance;
uniform float4   u_FogDepth;
uniform float  u_FogEyeT;
uniform float4   u_FogColorMask;
#endif

#if defined(USE_DEFORM_VERTEXES)
uniform int    u_DeformGen;
uniform float  u_DeformParams[5];
uniform float  u_Time;
#endif

uniform float4x4   u_ModelViewProjectionMatrix;
uniform float4   u_BaseColor;
uniform float4   u_VertColor;

#if defined(USE_RGBAGEN)
uniform int    u_ColorGen;
uniform int    u_AlphaGen;
uniform float3   u_AmbientLight;
uniform float3   u_DirectedLight;
uniform float3   u_ModelLightDir;
uniform float  u_PortalRange;
#endif

#if defined(USE_VERTEX_ANIMATION)
uniform float  u_VertexLerp;
#elif defined(USE_BONE_ANIMATION)
uniform float4x4 u_BoneMatrix[MAX_GLSL_BONES];
#endif

#if defined(USE_DEFORM_VERTEXES)
float3 DeformPosition(float3 pos, float3 normal, float2 st)
{
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
		func = sign(frac(0.5 - value));
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

#if defined(USE_TCGEN)
float2 GenTexCoords(int TCGen, float3 position, float3 normal, float3 TCGenVector0, float3 TCGenVector1, float4 attr_TexCoord0, float4 attr_TexCoord1)
{
	float2 tex = attr_TexCoord0.xy;

	if (TCGen == TCGEN_LIGHTMAP)
	{
		tex = attr_TexCoord1.xy;
	}
	else if (TCGen == TCGEN_ENVIRONMENT_MAPPED)
	{
		float3 viewer = normalize(u_LocalViewOrigin - position);
		float2 ref = reflect(viewer, normal).yz;
		tex.x = ref.x * -0.5 + 0.5;
		tex.y = ref.y *  0.5 + 0.5;
	}
	else if (TCGen == TCGEN_VECTOR)
	{
		tex = float2(dot(position, TCGenVector0), dot(position, TCGenVector1));
	}
	
	return tex;
}
#endif

#if defined(USE_TCMOD)
float2 ModTexCoords(float2 st, float3 position, float4 texMatrix, float4 offTurb)
{
	float amplitude = offTurb.z;
	float phase = offTurb.w * 2.0 * M_PI;
	float2 st2;
	st2.x = st.x * texMatrix.x + (st.y * texMatrix.z + offTurb.x);
	st2.y = st.x * texMatrix.y + (st.y * texMatrix.w + offTurb.y);

	float2 offsetPos = float2(position.x + position.z, position.y);
	
	float2 texOffset = sin(offsetPos * (2.0 * M_PI / 1024.0) + float2(phase));
	
	return st2 + texOffset * amplitude;	
}
#endif

#if defined(USE_RGBAGEN)
float4 CalcColor(float3 position, float3 normal, float4 attr_Color)
{
	float4 color = u_VertColor * attr_Color + u_BaseColor;
	
	if (u_ColorGen == CGEN_LIGHTING_DIFFUSE)
	{
		float incoming = clamp(dot(normal, u_ModelLightDir), 0.0, 1.0);

		color.rgb = clamp(u_DirectedLight * incoming + u_AmbientLight, 0.0, 1.0);
	}
	
	float3 viewer = u_LocalViewOrigin - position;

	if (u_AlphaGen == AGEN_LIGHTING_SPECULAR)
	{
		float3 lightDir = normalize(float3(-960.0, 1980.0, 96.0) - position);
		float3 reflected = -reflect(lightDir, normal);
		
		color.a = clamp(dot(reflected, normalize(viewer)), 0.0, 1.0);
		color.a *= color.a;
		color.a *= color.a;
	}
	else if (u_AlphaGen == AGEN_PORTAL)
	{
		color.a = clamp(length(viewer) / u_PortalRange, 0.0, 1.0);
	}
	
	return color;
}
#endif

#if defined(USE_FOG)
float CalcFog(float3 position)
{
	float s = dot(float4(position, 1.0), u_FogDistance) * 8.0;
	float t = dot(float4(position, 1.0), u_FogDepth);

	float eyeOutside = float(u_FogEyeT < 0.0);
	float fogged = float(t >= eyeOutside);

	t += 1e-6;
	t *= fogged / (t - u_FogEyeT * eyeOutside);

	return s * t;
}
#endif

void main(
	float3 attr_Position,
	float3 attr_Normal,
#if defined(USE_VERTEX_ANIMATION)
	float3 attr_Position2,
	float3 attr_Normal2,
#elif defined(USE_BONE_ANIMATION)
	float4 attr_BoneIndexes,
	float4 attr_BoneWeights,
#endif
	float4 attr_Color,
	float4 attr_TexCoord0,
#if defined(USE_TCGEN)
	float4 attr_TexCoord1,
#endif
	float4 out gl_Position : POSITION,
	float4 out var_Color : COLOR,
	float2 out var_DiffuseTex : TEXCOORD0
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
	float3 position  = attr_Position;
	float3 normal    = attr_Normal;
#endif

#if defined(USE_DEFORM_VERTEXES)
	position = DeformPosition(position, normal, attr_TexCoord0.xy);
#endif

	gl_Position = mul(float4(position, 1.0), u_ModelViewProjectionMatrix);

#if defined(USE_TCGEN)
	float2 tex = GenTexCoords(u_TCGen0, position, normal, u_TCGen0Vector0, u_TCGen0Vector1, attr_TexCoord0, attr_TexCoord1);
#else
	float2 tex = attr_TexCoord0.xy;
#endif

#if defined(USE_TCMOD)
	var_DiffuseTex = ModTexCoords(tex, position, u_DiffuseTexMatrix, u_DiffuseTexOffTurb);
#else
    var_DiffuseTex = tex;
#endif

#if defined(USE_RGBAGEN)
	var_Color = CalcColor(position, normal, attr_Color);
#else
	var_Color = u_VertColor * attr_Color + u_BaseColor;
#endif

#if defined(USE_FOG)
	var_Color *= float4(1.0, 1.0, 1.0, 1.0) - u_FogColorMask * sqrt(clamp(CalcFog(position), 0.0, 1.0));
#endif
}
