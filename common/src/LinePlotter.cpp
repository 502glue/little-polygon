// Little Polygon SDK
// Copyright (C) 2013 Max Kaufmann
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "littlepolygon_graphics.h"

const GLchar* SIMPLE_SHADER = R"GLSL(

varying mediump vec4 color;

#if VERTEX

uniform mediump mat4 mvp;
attribute mediump vec2 aPosition;
attribute mediump vec4 aColor;

void main() {
	gl_Position = mvp * vec4(aPosition, 0, 1);
	color = aColor;
}

#else

void main() {
	gl_FragColor = color;
}

#endif

)GLSL";

// private helper functions
void commitBatch(LinePlotter *context);	

void initialize(LinePlotter* context) {
	context->count = -1;
	CHECK( compileShader(SIMPLE_SHADER, &context->prog, &context->vert, &context->frag) );
	glUseProgram(context->prog);
	context->uMVP = glGetUniformLocation(context->prog, "mvp");
	context->aPosition = glGetAttribLocation(context->prog, "aPosition");
	context->aColor = glGetAttribLocation(context->prog, "aColor");
}

void release(LinePlotter *context) {
	glDeleteProgram(context->prog);
	glDeleteShader(context->vert);
	glDeleteShader(context->frag);
}

void begin(LinePlotter *context, vec2 canvasSize, vec2 canvasOffset) {
	ASSERT(context->count == -1);
	context->count = 0;

	glDisable(GL_BLEND);
	glLineWidth(2);

	glUseProgram(context->prog);

	setCanvas(context->uMVP, canvasSize, canvasOffset);

	glEnableVertexAttribArray(context->aPosition);
	glEnableVertexAttribArray(context->aColor);

	glVertexAttribPointer(context->aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(LinePlotter::Vertex), &context->vertices[0].position);
	glVertexAttribPointer(context->aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(LinePlotter::Vertex), &context->vertices[0].color);	
}

void plot(LinePlotter *context, vec2 p0, vec2 p1, Color c) {
	ASSERT(context->count >= 0);
	context->vertices[2*context->count  ].set(p0, c);
	context->vertices[2*context->count+1].set(p1, c);

	++context->count;
	if (context->count == LINE_PLOTTER_CAPACITY) {
		commitBatch(context);
	}
}

void end(LinePlotter *context) {
	ASSERT(context->count >= 0);
	if (context->count > 0) {
		commitBatch(context);
	}
	context->count = -1;
	glDisableVertexAttribArray(context->aPosition);
	glDisableVertexAttribArray(context->aColor);
	glUseProgram(0);
}

void commitBatch(LinePlotter *context) {
	ASSERT(context->count > 0);
	glDrawArrays(GL_LINES, 0, 2*context->count);
	context->count = 0;
}
