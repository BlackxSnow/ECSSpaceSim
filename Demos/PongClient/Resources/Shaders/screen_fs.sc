$input v_uv0
#include <bgfx_shader.sh>

SAMPLER2D(texColour, 0);

void main()
{
    // gl_FragColor = vec4(1,1,0,1);
    gl_FragColor = texture2D(texColour, v_uv0);
}