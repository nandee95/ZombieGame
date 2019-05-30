uniform sampler2D texture;
in vec4 Color;
in vec2 texCoords;
void main()
{
    gl_FragColor = texture2D(texture, texCoords) * Color;
}