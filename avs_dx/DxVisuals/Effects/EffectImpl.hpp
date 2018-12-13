#pragma once
#include "../EffectBase/EffectBase.h"
#include "../Render/EffectRenderer.hpp"
#include "../Resources/RenderTargets.hpp"
#include "../Resources/staticResources.h"
#include "../Resources/StructureBuffer.h"
#include "shadersCode.h"
#include "../Hlsl/Defines.h"
#include "EffectBase1.hpp"
using Hlsl::Defines;

// Interop with the AVS code
inline int min( int a, int b )
{
	return std::min( a, b );
}
inline int max( int a, int b )
{
	return std::max( a, b );
}
#include "../../avs/vis_avs/r_defs.h"
#include "../InteropLib/effectsFactory.h"

template<class TEffect>
class EffectImpl : public TEffect
{
public:
	EffectImpl( void* pNative ) : TEffect( ( typename TEffect::AvsState* )( pNative ) ) { }

	~EffectImpl() override = default;

	static inline HRESULT create( void* pState, std::unique_ptr<iEffect>& res )
	{
		using tImpl = EffectImpl<TEffect>;
		res = std::make_unique<tImpl>( pState );
		return S_OK;
	}
};

#define DECLARE_EFFECT( DX )                   \
const Metadata& metadata() override;

#define IMPLEMENT_EFFECT( DX, NATIVE )                                                     \
class NATIVE;                                                                              \
template<> HRESULT createDxEffect<NATIVE>( void* pState, std::unique_ptr<iEffect>& dest )  \
{                                                                                          \
	return EffectImpl<DX>::create( pState, dest );                                         \
};                                                                                         \
static const EffectBase::Metadata s_metadada{ #DX, false };                                \
const EffectBase::Metadata& DX::metadata(){ return s_metadada; }