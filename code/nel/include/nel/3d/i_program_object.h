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


#ifndef I_PROGRAM_OBJECT_H
#define I_PROGRAM_OBJECT_H

namespace NL3D
{

	class IProgram;

	class IProgramObject
	{
	public:

		enum EUniform
		{
			MVPMatrix,
			MVMatrix,
			NormalMatrix,
			TexMatrix0,
			TexMatrix1,
			TexMatrix2,
			TexMatrix3,
			Constant0,
			Constant1,
			Constant2,
			Constant3,
			Diffuse,
			Color,
			Sampler0,
			Sampler1,
			Sampler2,
			Sampler3,
			AlphaTreshold,
			FogStart,
			FogEnd,
			FogColor,
			FogDensity,
			Light0Dir,
			Light1Dir,
			Light2Dir,
			Light3Dir,
			Light4Dir,
			Light5Dir,
			Light6Dir,
			Light7Dir,
			Light0ColDiff,
			Light1ColDiff,
			Light2ColDiff,
			Light3ColDiff,
			Light4ColDiff,
			Light5ColDiff,
			Light6ColDiff,
			Light7ColDiff,
			Light0ColAmb,
			Light1ColAmb,
			Light2ColAmb,
			Light3ColAmb,
			Light4ColAmb,
			Light5ColAmb,
			Light6ColAmb,
			Light7ColAmb,
			Light0ColSpec,
			Light1ColSpec,
			Light2ColSpec,
			Light3ColSpec,
			Light4ColSpec,
			Light5ColSpec,
			Light6ColSpec,
			Light7ColSpec,
			Light0Shininess,
			Light1Shininess,
			Light2Shininess,
			Light3Shininess,
			Light4Shininess,
			Light5Shininess,
			Light6Shininess,
			Light7Shininess,
			Light0Pos,
			Light1Pos,
			Light2Pos,
			Light3Pos,
			Light4Pos,
			Light5Pos,
			Light6Pos,
			Light7Pos,
			Light0ConstAttn,
			Light1ConstAttn,
			Light2ConstAttn,
			Light3ConstAttn,
			Light4ConstAttn,
			Light5ConstAttn,
			Light6ConstAttn,
			Light7ConstAttn,
			Light0LinAttn,
			Light1LinAttn,
			Light2LinAttn,
			Light3LinAttn,
			Light4LinAttn,
			Light5LinAttn,
			Light6LinAttn,
			Light7LinAttn,
			Light0QuadAttn,
			Light1QuadAttn,
			Light2QuadAttn,
			Light3QuadAttn,
			Light4QuadAttn,
			Light5QuadAttn,
			Light6QuadAttn,
			Light7QuadAttn,
			NUM_UNIFORMS
		};

		IProgramObject(){}

		virtual ~IProgramObject(){}

		virtual bool attachVertexProgram( IProgram *shader ) = 0;

		virtual bool attachPixelProgram( IProgram *shader ) = 0;

		virtual bool detachVertexProgram( IProgram *shader ) = 0;

		virtual bool detachPixelProgram( IProgram *shader ) = 0;

		unsigned int getProgramId() const{ return programId; }

		virtual bool link( std::string &log ) = 0;

		virtual bool validate( std::string &log ) = 0;

		virtual void cacheUniformIndices() = 0;

		virtual int getUniformIndex( EUniform uniform ) = 0;

		bool isLinked() const{ return linked; }

	protected:
		unsigned int programId;
		bool linked;
	};

}

#endif

