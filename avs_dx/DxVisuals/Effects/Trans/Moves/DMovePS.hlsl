#ifndef AVS_SHADER
#define BIND_SAMPLER s1
// <macro-resource name="bindSampler" macro="BIND_SAMPLER" type="sampler" />
static const bool blending = true;
#endif
Texture2D<float4> texPrevFrame : register(t3);
SamplerState ss : register(BIND_SAMPLER);

float4 main( float3 tc : TEXCOORD0 ) : SV_Target
{
    float4 res = texPrevFrame.Sample( ss, tc.xy );
    if( blending )
        res = float4( res.rgb * tc.z, tc.z );
    return res;
}