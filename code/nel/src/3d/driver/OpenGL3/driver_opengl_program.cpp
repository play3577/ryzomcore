// NeL - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "driver_opengl.h"
#include "driver_glsl_program.h"
#include "driver_glsl_vertex_program.h"
#include "driver_glsl_pixel_program.h"
#include "driver_glsl_shader_generator.h"
#include "driver_opengl_vertex_buffer_hard.h"
#include "nel/3d/i_program_object.h"

namespace
{
	const char *constNames[ NL3D::IDRV_MAT_MAXTEXTURES ] =
	{
		"constant0",
		"constant1",
		"constant2",
		"constant3"
	};

}

namespace NL3D
{
	bool CDriverGL3::activeProgramObject( IProgramObject *program )
	{
		if( !program )
		{
#ifndef GLSL
			_VertexProgramEnabled = false;
#endif
			currentProgram = NULL;
			return true;
		}

		if( !program->isLinked() )
			return false;

		// Release previous program
		releaseProgram();
		
		nglUseProgram( program->getProgramId() );

		GLenum error = glGetError();
		if( error != GL_NO_ERROR )
			return false;

		_VertexProgramEnabled = true;
		currentProgram = program;

		return true;
	}


	IProgramObject* CDriverGL3::createProgramObject() const
	{
		return new CGLSLProgram();
	}

	IProgram* CDriverGL3::createVertexProgram() const
	{
		return new CGLSLVertexProgram();
	}

	IProgram* CDriverGL3::createPixelProgram() const
	{
		return new CGLSLPixelProgram();
	}

	int CDriverGL3::getUniformLocation( const char *name )
	{
		if( currentProgram == NULL )
			return -1;

		return nglGetUniformLocation( currentProgram->getProgramId(), name );
	}


	void CDriverGL3::setUniform1f( uint index, float f )
	{
		nglUniform1f( index, f );
	}

	void CDriverGL3::setUniform3f( uint index, float f1, float f2, float f3 )
	{
		nglUniform3f( index, f1, f2, f3 );
	}

	void CDriverGL3::setUniform4f( uint index, float f1, float f2, float f3, float f4 )
	{
		nglUniform4f( index, f1, f2, f3, f4 );
	}

	void CDriverGL3::setUniform1i( uint index, int i )
	{
		nglUniform1i( index, i );
	}

	void CDriverGL3::setUniform4i( uint index, int i1, int i2, int i3, int i4 )
	{
		nglUniform4i( index, i1, i2, i3, i4 );
	}

	void CDriverGL3::setUniform1u( uint index, uint u )
	{
		nglUniform1ui( index, u );
	}

	void CDriverGL3::setUniform4u( uint index, uint u1, uint u2, uint u3, uint u4 )
	{
		nglUniform4ui( index, u1, u2, u3, u4 );
	}

	void CDriverGL3::setUniformMatrix2fv( uint index, uint count, bool transpose, const float *values )
	{
		nglUniformMatrix2fv( index, count, transpose, values );
	}

	void CDriverGL3::setUniformMatrix3fv( uint index, uint count, bool transpose, const float *values )
	{
		nglUniformMatrix3fv( index, count, transpose, values );
	}

	void CDriverGL3::setUniformMatrix4fv( uint index, uint count, bool transpose, const float *values )
	{
		nglUniformMatrix4fv( index, count, transpose, values );
	}

	bool CDriverGL3::renderRawTriangles2( CMaterial &mat, uint32 startIndex, uint32 numTris )
	{
		glDrawArrays( GL_TRIANGLES, startIndex * 3, numTris * 3 );

		return true;
	}

	static IProgram *vp;
	static IProgram *pp;
	static IProgramObject *p;


	void CDriverGL3::generateShaderDesc( CShaderDesc &desc, CMaterial &mat )
	{
		desc.setShaderType( mat.getShader() );
		desc.setVBFlags( _CurrentVertexBufferHard->VB->getVertexFormat() );
		
		if( mat.getShader() == CMaterial::LightMap )
			desc.setNLightMaps( mat._LightMaps.size() );
		
		int i = 0;

		if( mat.getShader() == CMaterial::Normal )
		{
			int maxTextures = std::min( int( SHADER_MAX_TEXTURES ), int( IDRV_MAT_MAXTEXTURES ) );
			for( i = 0; i < maxTextures; i++ )
			{
				desc.setTexEnvMode( i, mat.getTexEnvMode( i ) );
			}
		}

		if( mat.getAlphaTest() )
		{
			desc.setAlphaTest( true );
			desc.setAlphaTestThreshold( mat.getAlphaTestThreshold() );
		}

		if( fogEnabled() )
		{
			desc.setFog( true );
			desc.setFogMode( CShaderDesc::Linear );
		}

		int maxLights = std::min( int( SHADER_MAX_LIGHTS ), int( MaxLight ) );
		bool enableLights = false;
		for( int i = 0; i < maxLights; i++ )
		{
			if( !_UserLightEnable[ i ] )
				continue;

			enableLights = true;
			
			switch( _LightMode[ i ] )
			{
			case CLight::DirectionalLight:
				desc.setLight( i, CShaderDesc::Directional );
				break;
			
			case CLight::PointLight:
				desc.setLight( i, CShaderDesc::Point );
				break;
			
			case CLight::SpotLight:
				desc.setLight( i, CShaderDesc::Spot );
				break;
			}
		
		}

		desc.setLighting( enableLights );			
	}


	bool CDriverGL3::setupProgram( CMaterial &mat )
	{

#ifdef GLSL
		CShaderDesc desc;

		generateShaderDesc( desc, mat );

		p = shaderCache.findShader( desc );

		if( p != NULL )
		{
			if( !activeProgramObject( p ) )
				return false;
		}
		else
		{
			std::string vs;
			std::string ps;

			shaderGenerator->reset();
			shaderGenerator->setMaterial( &mat );
			shaderGenerator->setVBFormat( _CurrentVertexBufferHard->VB->getVertexFormat() );
			shaderGenerator->setShaderDesc( &desc );
			shaderGenerator->generateVS( vs );
			shaderGenerator->generatePS( ps );

			vp = createVertexProgram();
			std::string log;

			vp->shaderSource( vs.c_str() );
			if( !vp->compile( log ) )
			{
				delete vp;
				vp = NULL;
				nlinfo( "%s", log.c_str() );
				return false;
			}
		
			pp = createPixelProgram();
			pp->shaderSource( ps.c_str() );
			if( !pp->compile( log ) )
			{
				delete vp;
				vp = NULL;
				delete pp;
				pp = NULL;
				nlinfo( "%s", log.c_str() );
				return false;
			}

			p = createProgramObject();
			p->attachVertexProgram( vp );
			p->attachPixelProgram( pp );
			if( !p->link( log ) )
			{
				vp = NULL;
				pp = NULL;
				delete p;
				p = NULL;
				nlinfo( "%s", log.c_str() );
				return false;
			}

			if( !activeProgramObject( p ) )
				return false;
			
			p->cacheUniformIndices();
			desc.setProgram( p );
			shaderCache.cacheShader( desc );
		}

		setupUniforms( mat );

#endif

		return true;
	}


	void CDriverGL3::setupUniforms( CMaterial& mat )
	{

#ifdef GLSL

		int mvpIndex = currentProgram->getUniformIndex( IProgramObject::MVPMatrix );
		if( mvpIndex != -1 )
		{
			CMatrix mat = _GLProjMat * _ModelViewMatrix;
			setUniformMatrix4fv( mvpIndex, 1, false, mat.get() );
		}

		int mvIndex = currentProgram->getUniformIndex( IProgramObject::MVMatrix );
		if( mvIndex != -1 )
		{
			setUniformMatrix4fv( mvIndex, 1, false, _ModelViewMatrix.get() );
		}

		/*
		int nmIdx = currentProgram->getUniformIndex( IProgramObject::NormalMatrix );
		if( nmIdx != -1 )
		{
		}
		*/

		int fogStartIdx = currentProgram->getUniformIndex( IProgramObject::FogStart );
		if( fogStartIdx != -1 )
		{
			setUniform1f( fogStartIdx, getFogStart() );
		}

		int fogEndIdx = currentProgram->getUniformIndex( IProgramObject::FogEnd );
		if( fogEndIdx != -1 )
		{
			setUniform1f( fogEndIdx, getFogEnd() );
		}

		int fogColorIdx = currentProgram->getUniformIndex( IProgramObject::FogColor );
		if( fogColorIdx != -1 )
		{
			GLfloat glCol[ 4 ];
			CRGBA col = getFogColor();
			glCol[ 0 ] = col.R / 255.0f;
			glCol[ 1 ] = col.G / 255.0f;
			glCol[ 2 ] = col.B / 255.0f;
			glCol[ 3 ] = col.A / 255.0f;
			setUniform4f( fogColorIdx, glCol[ 0 ], glCol[ 1 ], glCol[ 2 ], glCol[ 3 ] );
		}

		int colorIndex = currentProgram->getUniformIndex( IProgramObject::Color );
		if( colorIndex != -1 )
		{
			GLfloat glCol[ 4 ];
			CRGBA col = mat.getColor();
			glCol[ 0 ] = col.R / 255.0f;
			glCol[ 1 ] = col.G / 255.0f;
			glCol[ 2 ] = col.B / 255.0f;
			glCol[ 3 ] = col.A / 255.0f;

			setUniform4f( colorIndex, glCol[ 0 ], glCol[ 1 ], glCol[ 2 ], glCol[ 3 ] );
		}

		int diffuseIndex = currentProgram->getUniformIndex( IProgramObject::Diffuse );
		if( diffuseIndex != -1 )
		{
			GLfloat glCol[ 4 ];
			CRGBA col = mat.getDiffuse();
			glCol[ 0 ] = col.R / 255.0f;
			glCol[ 1 ] = col.G / 255.0f;
			glCol[ 2 ] = col.B / 255.0f;
			glCol[ 3 ] = col.A / 255.0f;

			setUniform4f( diffuseIndex, glCol[ 0 ], glCol[ 1 ], glCol[ 2 ], glCol[ 3 ] );
		}


		int maxLights = std::min( int( MaxLight ), int( SHADER_MAX_LIGHTS ) );
		for( int i = 0; i < maxLights; i++ )
		{
			if( !_UserLightEnable[ i ] )
				continue;
			
			////////////////// Temporary insanity  ///////////////////////////////
			if( _LightMode[ i ] != CLight::DirectionalLight && _LightMode[ i ] != CLight::PointLight )
				continue;
			//////////////////////////////////////////////////////////////////////
			
			int ld = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0Dir + i ) );
			if( ld != -1 )
			{
				CVector v = _UserLight[ i ].getDirection();
				setUniform3f( ld, v.x, v.y, v.z );
			}

			int lp = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0Pos + i ) );
			if( lp != -1 )
			{
				CVector v = _UserLight[ i ].getPosition();
				setUniform3f( lp, v.x, v.y, v.z );
			}

			int ldc = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0ColDiff + i ) );
			if( ldc != -1 )
			{
				GLfloat glCol[ 4 ];
				CRGBA col = _UserLight[ i ].getDiffuse();
				glCol[ 0 ] = col.R / 255.0f;
				glCol[ 1 ] = col.G / 255.0f;
				glCol[ 2 ] = col.B / 255.0f;
				glCol[ 3 ] = col.A / 255.0f;
				setUniform4f( ldc, glCol[ 0 ], glCol[ 1 ], glCol[ 2 ], glCol[ 3 ] );
			}

			int lsc = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0ColSpec + i ) );
			if( lsc != -1 )
			{
				GLfloat glCol[ 4 ];
				CRGBA col = _UserLight[ i ].getSpecular();
				glCol[ 0 ] = col.R / 255.0f;
				glCol[ 1 ] = col.G / 255.0f;
				glCol[ 2 ] = col.B / 255.0f;
				glCol[ 3 ] = col.A / 255.0f;
				setUniform4f( lsc, glCol[ 0 ], glCol[ 1 ], glCol[ 2 ], glCol[ 3 ] );
			}

			int shl = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0Shininess + i ) );
			if( shl != -1 )
			{
				setUniform1f( shl, mat.getShininess() );
			}

			int lac = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0ColAmb + i ) );
			if( lac != -1 )
			{
				GLfloat glCol[ 4 ];
				CRGBA col;
				if( mat.getShader() == CMaterial::LightMap )
					col = _UserLight[ i ].getAmbiant();
				else
					col.add( _UserLight[ i ].getAmbiant(), mat.getEmissive() );

				glCol[ 0 ] = col.R / 255.0f;
				glCol[ 1 ] = col.G / 255.0f;
				glCol[ 2 ] = col.B / 255.0f;
				glCol[ 3 ] = col.A / 255.0f;
				setUniform4f( lac, glCol[ 0 ], glCol[ 1 ], glCol[ 2 ], glCol[ 3 ] );
			}

			int lca = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0ConstAttn + i ) );
			if( lca != -1 )
			{
				setUniform1f( lca, _UserLight[ i ].getConstantAttenuation() );
			}

			int lla = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0LinAttn + i ) );
			if( lla != -1 )
			{
				setUniform1f( lla, _UserLight[ i ].getLinearAttenuation() );
			}

			int lqa = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Light0QuadAttn + i ) );
			if( lqa != -1 )
			{
				setUniform1f( lqa, _UserLight[ i ].getQuadraticAttenuation() );
			}
		}


		// Lightmaps have special constants
		if( mat.getShader() != CMaterial::LightMap )
		{
		
			for( int i = 0; i < IDRV_MAT_MAXTEXTURES; i++ )
			{
				int cl = currentProgram->getUniformIndex( IProgramObject::EUniform( IProgramObject::Constant0 + i ) );
				if( cl != -1 )
				{
					CRGBA col = mat._TexEnvs[ i ].ConstantColor;
					GLfloat glCol[ 4 ];
					glCol[ 0 ] = col.R / 255.0f;
					glCol[ 1 ] = col.G / 255.0f;
					glCol[ 2 ] = col.B / 255.0f;
					glCol[ 3 ] = col.A / 255.0f;

					setUniform4f( cl, glCol[ 0 ], glCol[ 1 ], glCol[ 2 ], glCol[ 3 ] );
				}
			}

		}

#endif
	}

	void CDriverGL3::releaseProgram()
	{
		currentProgram = NULL;

#ifndef GLSL
		_VertexProgramEnabled = false;
#endif
	}

}


