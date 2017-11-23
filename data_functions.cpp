#include "data_structs.h"
#include <ctype.h>

char line[1024];
int linecount = 0;
char texture[64];
//////////////////////////////////////////////////////////////////////////
// Load SMD file
// input :
//		- char* path (Complete filepath of the SMD file to load)
//		- smd_model* pModel (Pointer to the model structure to load the file in)
// output :
//		- int (Error message, result of the function)
//////////////////////////////////////////////////////////////////////////
int LoadSMD( char* path, smd_model* pModel )
{
	FILE* pfile;
	char cmd[1024];
	int option;
	int res;

	if ( !path || !pModel )
		return NULL_POINTER;

	if ( (pfile = fopen(path, "r")) == 0 )
		return WRONG_FILE;

	
	while (fgets( line, sizeof( line ), pfile ) != NULL)
	{
		linecount++;
		sscanf( line, "%s %d", cmd, &option );
		if ( strcmp(cmd, "version") == 0 )
		{
			if ( option != 1 )
				return WRONG_VERSION;
		}
		else if ( strcmp(cmd, "nodes") == 0 )
		{
			res = LoadNodes(pfile, pModel);
			if ( res != ALL_OK )
				return res;
		}
		else if ( strcmp(cmd, "skeleton") == 0 )
		{
			res = LoadSkeleton(pfile, pModel);
			if ( res != ALL_OK )
				return res;
		}
		else if ( strcmp(cmd, "triangles") == 0 )
		{
			res = LoadTriangles(pfile, pModel);
			if ( res != ALL_OK )
				return res;
		}
	}
	fclose( pfile );
	return linecount;
}

//////////////////////////////////////////////////////////////////////////
// Load Nodes from File
// input :
//		- FILE* pfile (Pointer to the opened file to load the nodes from)
//		- smd_model* pModel (Pointer to the model to put the nodes in)
// output :
//		- int (Error message, result of the function)
//////////////////////////////////////////////////////////////////////////
int LoadNodes( FILE* pfile, smd_model* pModel )
{
	int i = 0;

	while (fgets( line, sizeof( line ), pfile ) != NULL)
	{
		linecount++;
		if ( strcmp(line, "end\n") == 0 )
		{
			printf( "%i nodes loaded\n", i );
			return ALL_OK;
		}

		AddNode( line, pModel );
		i++;
	}
	return END_OF_FILE;
}

//////////////////////////////////////////////////////////////////////////
// Load Bones(Skeleton) from File
// input :
//		- FILE* pfile (Pointer to the opened file to load the bones from)
//		- smd_model* pModel (Pointer to the model to put the bones in)
// output :
//		- int (Error message, result of the function)
//////////////////////////////////////////////////////////////////////////
int LoadSkeleton( FILE* pfile, smd_model* pModel )
{
	int i = 0;

	// Time command
	fgets( line, sizeof( line ), pfile );
	linecount++;


	
	while (fgets( line, sizeof( line ), pfile ) != NULL)
	{
		linecount++;
		if ( strcmp(line, "end\n") == 0 )
		{
			printf( "%i bones loaded\n", i );
			return ALL_OK;
		}

		AddBone( line, pModel );
		i++;
	}
	return END_OF_FILE;
}

//////////////////////////////////////////////////////////////////////////
// Load Traingles from File
// input :
//		- FILE* pfile (Pointer to the opened file to load the triangles from)
//		- smd_model* pModel (Pointer to the model to put the triangles in)
// output :
//		- int (Error message, result of the function)
//////////////////////////////////////////////////////////////////////////
int LoadTriangles( FILE* pfile, smd_model* pModel )
{
	smd_mesh* pMesh;
	int i = 0;
	int k = 0;

	while (fgets( line, sizeof( line ), pfile ) != NULL)
	{
		linecount++;
		if ( strcmp(line, "end\n") == 0 )
		{
			printf( "%i triangles loaded\n", i );
			return ALL_OK;
		}

		strcpy(texture, line);
		pMesh = GetMesh( line, pModel );
		
		smd_triangle* p;
		p = (smd_triangle*)calloc( 1, sizeof(smd_triangle) );
		for ( int j = 0; j < 3; j++ )
		{
			fgets( line, sizeof( line ), pfile );
			linecount++;

			if ( sscanf( line, "%d %f %f %f %f %f %f %f %f",
							&p->vertexes[j].bone, 
							&p->vertexes[j].vertpos[0], &p->vertexes[j].vertpos[1], &p->vertexes[j].vertpos[2], 
							&p->vertexes[j].normpos[0], &p->vertexes[j].normpos[1], &p->vertexes[j].normpos[2], 
							&p->vertexes[j].u, &p->vertexes[j].v ) != 9 ) 
			{
				return WRONG_LINE;
			}
			i++;
		}
		AddTriangle( pMesh, p );
		k++;
	}
	return END_OF_FILE;
}

//////////////////////////////////////////////////////////////////////////
// Load Bones(Skeleton) from File
// input :
//		- FILE* pfile (Pointer to the opened file to load the bones from)
//		- smd_model* pModel (Pointer to the model to put the bones in)
// output :
//		- int (Error message, result of the function)
//////////////////////////////////////////////////////////////////////////
int AddNode( char* input, smd_model* pModel )
{
	int index;
	int parent;
	char name[64];
	smd_node* pnode;

	if ( sscanf( input, "%d \"%[^\"]\" %d", &index, name, &parent ) == 3 )
	{
		if ( index > MAX_BONE )
		{
			return -1;
		}
		pnode = new smd_node;
		strcpy( pnode->name, name );
		pnode->parent = parent;
		pnode->pnext = NULL;
		
		if ( pModel->end_node )
			pModel->end_node->pnext = pnode;
		pModel->end_node = pnode;

		if ( pModel->start_node == NULL )
			pModel->start_node = pnode;

		pModel->numnodes++;
		return index;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// Load Bones(Skeleton) from File
// input :
//		- FILE* pfile (Pointer to the opened file to load the bones from)
//		- smd_model* pModel (Pointer to the model to put the bones in)
// output :
//		- int (Error message, result of the function)
//////////////////////////////////////////////////////////////////////////
int AddBone( char* input, smd_model* pModel )
{
	int index;
	vec3_f pos, rot;
	smd_bone* pbone;

	if ( sscanf( input, "%d %f %f %f %f %f %f", &index, &pos[0], &pos[1], &pos[2], &rot[0], &rot[1], &rot[2] ) == 7 )
	{
		if ( index > pModel->numnodes )
		{
			return -1;
		}
		pbone = new smd_bone;
		
		pbone->pos[0] = pos[0];
		pbone->pos[1] = pos[1];
		pbone->pos[2] = pos[2];

		pbone->rot[0] = rot[0];
		pbone->rot[1] = rot[1];
		pbone->rot[2] = rot[2];

		pbone->index = index;

		if ( pModel->end_bone )
			pModel->end_bone->pnext = pbone;
		pModel->end_bone = pbone;

		if ( pModel->start_bone == NULL )
			pModel->start_bone = pbone;
		return index;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// Add a triangle to the given mesh
// input :
//		- smd_mesh* pmesh (Pointer to the mesh to contain the new triangle)
//		- smd_triangle* ptriangle (Pointer to triangle to add)
// output :
//		- int (Error message, result of the function)
//////////////////////////////////////////////////////////////////////////
int AddTriangle( smd_mesh* pmesh, smd_triangle* ptriangle )
{
	if ( !pmesh )
		return -1;
	if ( !ptriangle )
		return -1;
	ptriangle->pnext = NULL;
	
	if ( pmesh->end_triangle )
		pmesh->end_triangle->pnext = ptriangle;
	pmesh->end_triangle = ptriangle;

	// check if it is the first triangle
	if ( pmesh->start_triangle == NULL )
		pmesh->start_triangle = ptriangle;

	pmesh->numtriangles++;
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// Get Mesh from Texture name
// input :
//		- char* texture (String with the texture name)
//		- smd_model* pModel (Pointer to the model to put the bones in)
// output :
//		- smd_mesh* (Pointer to the mesh found from the texture name)
//////////////////////////////////////////////////////////////////////////
smd_mesh* GetMesh( char* texture, smd_model* pModel )
{
	smd_texture* ptexture;
	smd_mesh* pmesh;

	ptexture = pModel->start_texture;
	for ( int i = 0; i < pModel->numtextures; i++ )
	{
		if ( strcmp(StripString(texture), ptexture->name) == 0 )
		{
			pmesh = pModel->start_mesh;
			for ( int j = 0; j < pModel->nummeshes; j++ )
			{
				if ( pmesh->texture == i )
				{
					return pmesh;
				}
				pmesh = pmesh->pnext;
			}
		}
		ptexture = ptexture->pnext;
	}

	// No MESH found

	ptexture = (smd_texture*)calloc( 1, sizeof(smd_texture) );
	pmesh = (smd_mesh*)calloc( 1, sizeof(smd_mesh) );
	
	// create new texture
	strcpy(ptexture->name, StripString(texture));
	ptexture->index = pModel->numtextures;
	ptexture->pnext = NULL;
	
	// create new mesh
	pmesh->texture = ptexture->index;
	pmesh->index = pModel->nummeshes;
	pmesh->pnext = NULL;

	// add mesh to model
	if ( pModel->end_mesh )
		pModel->end_mesh->pnext = pmesh;
	pModel->end_mesh = pmesh;

	// check is it is teh first mesh
	if ( pModel->start_mesh == NULL )
		pModel->start_mesh = pmesh;

	// add texture to model
	if ( pModel->end_texture )
		pModel->end_texture->pnext = ptexture;
	pModel->end_texture = ptexture;

	// check if it is the first texture
	if ( pModel->start_texture == NULL )
		pModel->start_texture = ptexture;
	
	pModel->numtextures++;
	pModel->nummeshes++;

	return pModel->end_mesh;
}

//////////////////////////////////////////////////////////////////////////
// Strip the string from newlines and spaces
// input :
//		- char* str (String to strip)
// output :
//		- char* (Striped string)
//////////////////////////////////////////////////////////////////////////
char* StripString( char* str )
{
	char* res;

	res = new char[strlen(str)];
	int j = 0;
	for ( size_t i = 0; i < strlen(str); i++ )
	{
		if ( str[i] != ' ' && str[i] != '\n' && str[i] != '\t' )
		{
			res[j] = str[i];
			j++;
		}
	}
	res[j] = '\0';
	return res;
}