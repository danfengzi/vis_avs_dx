#pragma once

namespace Hlsl
{
	// true if the character is a letter A to Z or a to z or an underscore
	inline bool isAlpha( char c )
	{
		return isalpha( c ) || c == '_';
	}

	// true if the character is a letter A to Z or a to z or an underscore or a digit
	inline bool isAlphaNumeric( char c )
	{
		return isalnum( c ) || c == '_';
	}

	// Enumerate all valid C identifiers in the string. The function argument must have the following prototype: bool gotId( int start, int length ), return false to continue parsing, true to quit.
	template<class TFunc>
	inline bool enumIdentifiers( const CStringA& code, int begin, int end, TFunc fnGotId )
	{
		const char* const src = code;
		end = std::min( code.GetLength(), end );

		for( int i = begin; i < end; )
		{
			if( !isAlpha( src[ i ] ) )
			{
				i++;
				continue;
			}
			const int idStart = i;

			for( i++; i < end; )
			{
				if( isAlphaNumeric( src[ i ] ) )
				{
					i++;
					continue;
				}
				break;
			}
			const int idLength = i - idStart;
			if( fnGotId( idStart, idLength ) )
				return true;
		}
		return false;
	}

	template<class TFunc>
	inline bool enumIdentifiers( const CStringA& code, TFunc fnGotId )
	{
		return enumIdentifiers( code, 0, code.GetLength(), fnGotId );
	}
}