#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D source_texture;


/**
  This shader is supposed to create the game of life iteration from a sourceTexture
  The final goal is to output this to a virtual frame that will be used to feed the next iteration, alongside display purpose
*/
void main()
{

    float self = texture(source_texture, TexCoord).r;

    if (self > 0.1) {
      FragColor = vec4(1.0, 0.0, 0.0, 1.0 );
    } else {
       FragColor = vec4(0.0, 0.0, 0.0, 1.0 );
    }


    //FragColor = vec4(self, 0.0, 0.0, 1.0 );

    return;

    //fetch each neighbor texel and current texel.
    //top row
    int r00 = int(textureOffset(source_texture, TexCoord, ivec2(-1, 1)).r);
    int r01 = int(textureOffset(source_texture, TexCoord, ivec2( 0, 1)).r);
    int r02 = int(textureOffset(source_texture, TexCoord, ivec2( 1, 1)).r);
 
    //middle row
    int r10 = int(textureOffset(source_texture, TexCoord, ivec2(-1, 0)).r);
    int r12 = int(textureOffset(source_texture, TexCoord, ivec2( 1, 0)).r);
   
    //bottom row
    int r20 = int(textureOffset(source_texture, TexCoord, ivec2(-1,-1)).r);
    int r21 = int(textureOffset(source_texture, TexCoord, ivec2( 0,-1)).r);
    int r22 = int(textureOffset(source_texture, TexCoord, ivec2( 1,-1)).r);
 
    int nb_neighbour =  r00 + r01 + r02 +
                        r10       + r12 +
                        r20 + r21 + r22;
 
    // staying alive
    if (self == 1 && (nb_neighbour == 2 || nb_neighbour == 3 )) {
      FragColor = vec4(1, 0, 0, 1);
    } else if (self == 0 && nb_neighbour == 3) {
      FragColor = vec4(1, 0, 0, 1);
    } else {
      FragColor = vec4(0, 0, 0, 1 );
    }
}