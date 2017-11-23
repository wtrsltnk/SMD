#include <windows.h>
#include "data_structs.h"

void main()
{
	smd_model* pModel;

	pModel = (smd_model*)calloc(1, sizeof(smd_model) );
	ZeroMemory( pModel, sizeof(smd_model) );

    int r = LoadSMD( "h:\\box.smd", pModel );

	switch ( r )
	{
	case ALL_OK:
		printf( "No error\n" );
		break;
	case WRONG_FILE:
		printf( "Wrong File\n" );
		break;
	case WRONG_LINE:
		printf( "Wrong Line\n" );
		break;
	case WRONG_VERSION:
		printf( "Wrong Version\n" );
		break;
	case END_OF_FILE:
		printf( "End Of File\n" );
		break;
	case NULL_POINTER:
		printf( "NULL Pointer\n" );
		break;
	default:
		printf( "%i lines\n", r );
	}

	if ( pModel )
        free(pModel);
}
