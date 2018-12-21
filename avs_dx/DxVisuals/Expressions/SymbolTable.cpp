#include "stdafx.h"
#include "SymbolTable.h"
#include "utils.hpp"
#include "Builtin/builtinFunctions.h"
#include "Builtin/includeFunctions.h"

using namespace Expressions;

SymbolTable::SymbolTable()
{
	addInternals();
}

SymbolTable::SymbolTable( const Prototype& proto )
{
	addInternals();

	proto.enumVariables( [ this ]( const VariableDecl& v )
	{
		const int id = (int)variables.size();
		variables.push_back( v );
		variablesMap[ v.name ] = id;
	} );
}

void SymbolTable::addInternals()
{
	int i = addFunc( "assign", eFunctionKind::Internal );
	assert( i == eInternalFunc::Assign );

	i = addFunc( "equal", eFunctionKind::Internal, eVarType::u32 );
	assert( i == eInternalFunc::Equals );

	i = addFunc( "if", eFunctionKind::Internal );
	assert( i == eInternalFunc::If );

	i = addFunc( "rand", eFunctionKind::Internal, eVarType::f32 );
	assert( i == eInternalFunc::Rand );
}

int SymbolTable::addFunc( const CStringA& name, eFunctionKind kind, eVarType vt )
{
	assert( nullptr == functionsMap.Lookup( name ) );
	const int id = (int)functions.size();
	functions.emplace_back( Function{ kind, vt, name } );
	functionsMap[ name ] = id;
	return id;
}

int SymbolTable::varLookup( const CStringA& name, eVarType& vt )
{
	auto p = variablesMap.Lookup( name );
	if( nullptr == p )
	{
		const int id = (int)variables.size();
		variables.emplace_back( VariableDecl{ eVarLocation::local, vt, name } );
		variablesMap[ name ] = id;
		return id;
	}

	VariableDecl& var = variables[ p->m_value ];
	int flag = 0;
	if( vt != eVarType::unknown )
		flag |= 1;
	if( var.vt != eVarType::unknown )
		flag |= 2;
	switch( flag )
	{
	case 1:
		var.vt = vt;
		break;
	case 2:
		vt = var.vt;
		break;
	case 3:
		if( var.vt != vt )
			logWarning( "Variable %s is re-declared as different type, old %s, new %s", cstr( name ), hlslName( var.vt ), hlslName( vt ) );
		break;
	}
	return p->m_value;
}

int SymbolTable::funcLookup( const CStringA& name, eVarType &vt )
{
	auto p = functionsMap.Lookup( name );
	if( nullptr != p )
	{
		const int id = p->m_value;
		const Function& func = functions[ id ];
		vt = func.vt;
		return id;
	}

	auto avs = lookupShaderFunc( name );
	if( nullptr != avs )
	{
		vt = avs->returnType;
		return addFunc( name, eFunctionKind::Avs, vt );
	}

	if( hasDoubleVersion( name, vt ) )
	{
		// sin/cos/tan are polymorphic however they have same return type as the built-in, i.e. it's known already
		if( vt != eVarType::f32 )
			vt = eVarType::unknown;
		return addFunc( name, eFunctionKind::Polymorphic, vt );
	}

	if( isBuiltinFunction( name, vt ) )
	{
		return addFunc( name, eFunctionKind::Hlsl, vt );
	}

	vt = eVarType::unknown;
	return addFunc( name, eFunctionKind::unknown, vt );
}

eVarType SymbolTable::varGetType( int id ) const
{
	return variables[ id ].vt;
}

eVarType SymbolTable::varSetType( int id, eVarType vt )
{
	auto& vtOld = variables[ id ].vt;

	int flag = 0;
	if( vt != eVarType::unknown )
		flag |= 1;
	if( vtOld != eVarType::unknown )
		flag |= 2;
	switch( flag )
	{
	case 1:
		vtOld = vt;
		return vt;
	case 2:
		return vtOld;
	case 3:
		if( vtOld != vt )
			logWarning( "Variable %s is reset to another type, old %s, new %s", cstr( variables[ id ].name ), hlslName( vtOld ), hlslName( vt ) );
		return vtOld;
	}
	return eVarType::unknown;
}

FunctionType SymbolTable::funGetType( int id ) const
{
	return functions[ id ];
}