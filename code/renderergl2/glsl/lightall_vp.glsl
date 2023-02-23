#if defined(USE_DELUXEMAP)
uniform float4   u_EnableTextures; // x = normal, y = deluxe, z = specular, w = cube
#endif

#if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
uniform float3   u_ViewOrigin;
#endif

#if defined(USE_TCGEN)
uniform int    u_TCGen0;
uniform float3   u_TCGen0Vector0;
uniform float3   u_TCGen0Vector1;
uniform float3   u_LocalViewOrigin;
#endif

#if defined(USE_TCMOD)
uniform float4   u_DiffuseTexMatrix;
uniform float4   u_DiffuseTexOffTurb;
#endif

uniform float4x4   u_ModelViewProjectionMatrix;
uniform float4   u_BaseColor;
uniform float4   u_VertColor;

#if defined(USE_MODELMATRIX)
uniform float4x4   u_ModelMatrix;
#endif

#if defined(USE_VERTEX_ANIMATION)
uniform float  u_VertexLerp;
#elif defined(USE_BONE_ANIMATION)
uniform float4x4 u_BoneMatrix[MAX_GLSL_BONES];
#endif

#if defined(USE_LIGHT_VECTOR)
uniform float4   u_LightOrigin;
uniform float  u_LightRadius;
uniform float3   u_DirectedLight;
uniform float3   u_AmbientLight;
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


float CalcLightAttenuation(float point, float normDist)
{
	// zero light at 1.0, approximating q3 style
	// also don't attenuate directional light
	float attenuation = (0.5 * normDist - 1.5) * point + 1.0;

	// clamp attenuation
	#if defined(NO_LIGHT_CLAMP)
	attenuation = max(attenuation, 0.0);
	#else
	attenuation = clamp(attenuation, 0.0, 1.0);
	#endif

	return attenuation;
}


void main(
	float4 attr_TexCoord0,
#if defined(USE_LIGHTMAP) || defined(USE_TCGEN)
	float4 attr_TexCoord1,
#endif
	float4 attr_Color,
	float3 attr_Position,
	float3 attr_Normal,
	float4 attr_Tangent,
#if defined(USE_VERTEX_ANIMATION)
	float3 attr_Position2,
	float3 attr_Normal2,
	float4 attr_Tangent2,
#elif defined(USE_BONE_ANIMATION)
	float4 attr_BoneIndexes,
	float4 attr_BoneWeights,
#endif
#if defined(USE_LIGHT) && !defined(USE_LIGHT_VECTOR)
	float3 attr_LightDirection,
#endif
	float4 out var_TexCoords : TEXCOORD0,
	float4 out var_Color : COLOR,
#if defined(USE_LIGHT_VECTOR) && !defined(USE_FAST_LIGHT)
	float4 out var_ColorAmbient : TEXCOORD1,
#endif
#if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float4 out var_Normal : TEXCOORD2,
	float4 out var_Tangent : TEXCOORD3,
	float4 out var_Bitangent : TEXCOORD4,
#endif
#if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float4 out var_LightDir :TEXCOORD5,
#endif
	float4 out gl_Position : POSITION
) {
#if defined(USE_VERTEX_ANIMATION)
	float3 position  = lerp(attr_Position,    attr_Position2,    u_VertexLerp);
	float3 normal    = lerp(attr_Normal,      attr_Normal2,      u_VertexLerp);
  #if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float3 tangent   = lerp(attr_Tangent.xyz, attr_Tangent2.xyz, u_VertexLerp);
  #endif
#elif defined(USE_BONE_ANIMATION)
	float4x4 vtxMat  = u_BoneMatrix[int(attr_BoneIndexes.x)] * attr_BoneWeights.x;
	     vtxMat += u_BoneMatrix[int(attr_BoneIndexes.y)] * attr_BoneWeights.y;
	     vtxMat += u_BoneMatrix[int(attr_BoneIndexes.z)] * attr_BoneWeights.z;
	     vtxMat += u_BoneMatrix[int(attr_BoneIndexes.w)] * attr_BoneWeights.w;
	float3x3 nrmMat = float3x3(cross(vtxMat[1].xyz, vtxMat[2].xyz), cross(vtxMat[2].xyz, vtxMat[0].xyz), cross(vtxMat[0].xyz, vtxMat[1].xyz));

	float3 position  = float3(mul(float4(attr_Position, 1.0), vtxMat));
	float3 normal    = normalize(mul(attr_Normal, nrmMat));
  #if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float3 tangent   = normalize(nrmMat * attr_Tangent.xyz);
  #endif
#else
	float3 position  = attr_Position;
	float3 normal    = attr_Normal;
  #if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float3 tangent   = attr_Tangent.xyz;
  #endif
#endif

#if defined(USE_TCGEN)
	float2 texCoords = GenTexCoords(u_TCGen0, position, normal, u_TCGen0Vector0, u_TCGen0Vector1, attr_TexCoord0, attr_TexCoord1);
#else
	float2 texCoords = attr_TexCoord0.xy;
#endif

#if defined(USE_TCMOD)
	var_TexCoords.xy = ModTexCoords(texCoords, position, u_DiffuseTexMatrix, u_DiffuseTexOffTurb);
#else
	var_TexCoords.xy = texCoords;
#endif

	gl_Position = mul(float4(position, 1.0), u_ModelViewProjectionMatrix);

#if defined(USE_MODELMATRIX)
	position  = (mul(float4(position, 1.0), u_ModelMatrix)).xyz;
	normal    = (mul(float4(normal,   0.0), u_ModelMatrix)).xyz;
  #if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	tangent   = (mul(float4(tangent,  0.0), u_ModelMatrix)).xyz;
  #endif
#endif

#if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float3 bitangent = cross(normal, tangent) * attr_Tangent.w;
#endif

#if defined(USE_LIGHT_VECTOR)
	float3 L = u_LightOrigin.xyz - (position * u_LightOrigin.w);
#elif defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float3 L = attr_LightDirection;
  #if defined(USE_MODELMATRIX)
	L = (mul(float4(L, 0.0), u_ModelMatrix)).xyz;
  #endif
#endif

#if defined(USE_LIGHTMAP)
	var_TexCoords.zw = attr_TexCoord1.xy;
#endif

	var_Color = u_VertColor * attr_Color + u_BaseColor;

#if defined(USE_LIGHT_VECTOR)
  #if defined(USE_FAST_LIGHT)
	float sqrLightDist = dot(L, L);
	float NL = clamp(dot(normalize(normal), L) / sqrt(sqrLightDist), 0.0, 1.0);
	float attenuation = CalcLightAttenuation(u_LightOrigin.w, u_LightRadius * u_LightRadius / sqrLightDist);

	var_Color.rgb *= u_DirectedLight * (attenuation * NL) + u_AmbientLight;
  #else
	var_ColorAmbient.rgb = u_AmbientLight * var_Color.rgb;
	var_Color.rgb *= u_DirectedLight;
    #if defined(USE_PBR)
	var_ColorAmbient.rgb *= var_ColorAmbient.rgb;
    #endif
  #endif
#endif

#if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT) && defined(USE_PBR)
	var_Color.rgb *= var_Color.rgb;
#endif

#if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
  #if defined(USE_LIGHT_VECTOR)
	var_LightDir = float4(L, u_LightRadius * u_LightRadius);
  #else
	var_LightDir = float4(L, 0.0);
  #endif
  #if defined(USE_DELUXEMAP)
	var_LightDir -= u_EnableTextures.y * var_LightDir;
  #endif
#endif

#if defined(USE_LIGHT) && !defined(USE_FAST_LIGHT)
	float3 viewDir = u_ViewOrigin - position;
	// store view direction in tangent space to save on varyings
	var_Normal    = float4(normal,    viewDir.x);
	var_Tangent   = float4(tangent,   viewDir.y);
	var_Bitangent = float4(bitangent, viewDir.z);
#endif
}
