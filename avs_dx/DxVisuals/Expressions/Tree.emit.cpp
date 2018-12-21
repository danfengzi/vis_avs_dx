#include "stdafx.h"
#include "Tree.h"
using namespace Expressions;

struct Tree::EmitContext
{
	CStringA& result;


	EmitContext( CStringA& hlsl ) :
		result( hlsl )
	{
		result = "";
		result.Preallocate( 1024 );
	}

	void lineBegin()
	{
		for( int i = 0; i < indent; i++ )
			result += '\t';
	}
	void lineEnd()
	{
		result += ";\r\n";
	}

	void code( const Node& node, const std::vector<char> &codez )
	{
		assert( node.node == Tree::eNode::Code );
		result.AppendFormat( "%.*s", node.length, codez.data() + node.id );
	}
	void operator+=( const CStringA& str )
	{
		result += str;
	}
private:
	int indent = 1;
};

void Tree::emitNode( EmitContext& ec, int ind ) const
{
	const Node& node = m_nodes[ ind ];
	const eNode nt = node.node;

	switch( nt )
	{
	case eNode::Code:
		ec.code( node, m_codez );
		return;
	case eNode::Var:
		ec += symbols.varName( node.id );
		return;
	case eNode::Expr:
		for( int i = ind + 1; true; )
		{
			emitNode( ec, i );
			if( !nextSibling( i ) )
				break;
		}
		return;
	}

	emitFunction( ec, ind );
}

void Tree::emitFunction( EmitContext& ec, int ind ) const
{
	const Node& node = m_nodes[ ind ];
	assert( node.node == eNode::Func );

	// Handle built-in functions
	if( node.id == SymbolTable::idAssign )
	{
		if( node.length != 2 )
			throw std::invalid_argument( "assign() must have exactly 2 arguments" );
		int i = ind + 1;
		emitNode( ec, i );
		ec += " = ";
		nextSibling( i );
		emitNode( ec, i );
		return;
	}

	if( node.id == SymbolTable::idEquals )
	{
		if( node.length != 2 )
			throw std::invalid_argument( "equals() must have exactly 2 arguments" );
		int i = ind + 1;
		emitNode( ec, i );
		ec += " == ";
		nextSibling( i );
		emitNode( ec, i );
		return;
	}

	if( node.id == SymbolTable::idIf )
	{
		if( node.length != 3 )
			throw std::invalid_argument( "if() must have exactly 3 arguments" );
		ec += "( ";
		int i = ind + 1;
		emitNode( ec, i );
		ec += " ) ? ";
		nextSibling( i );
		emitNode( ec, i );
		ec += " : ";
		nextSibling( i );
		emitNode( ec, i );
		return;
	}

	if( node.id == SymbolTable::idRand )
		throw std::invalid_argument( "You must call Tree::transformRandoms() to handle RNG" );

	ec += symbols.funcName( node.id );
	if( 0 == node.length )
	{
		ec += "()";
		return;
	}

	ec += "( ";
	for( int i = ind + 1; true; )
	{
		if( i != ind + 1 )
			ec += ", ";
		emitNode( ec, i );
		if( !nextSibling( i ) )
			break;
	}
	ec += " )";
}

HRESULT Tree::emitHlsl( CStringA& hlsl ) const
{
	try
	{
		EmitContext ec{ hlsl };
		for( int i = 0; i >= 0; )
		{
			// TODO: search for the stupid trick below, and emit top-level "if" instead of nested-level `operator ?`
			// From util.cpp:265
			// a little trick: assign(if(v,a,b),1.0); is like if V is true, a=1.0, otherwise b=1.0. :)

			ec.lineBegin();
			emitNode( ec, i );
			ec.lineEnd();
			i = m_nodes[ i ].nextSibling;
		}
		return S_OK;
	}
	catch( const std::exception& ex )
	{
		logError( "emitHlsl failed: %s", ex.what() );
		return E_FAIL;
	}
}