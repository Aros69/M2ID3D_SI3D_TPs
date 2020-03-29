#version 430

#ifdef COMPUTE_SHADER

// Definition de la texture
layout(binding = 0, r32ui) coherent uniform uimage2D imageMin;

// Taille des groupes de threads en 3d
layout(local_size_x= 64, local_size_y= 1, local_size_z=1) in;
void main() {
    // ATTENTION : le code si dessous ne prends pas en compte le fait que plus de
    // threads que necessaire sont lance (tester si la position global du thread
    // est plus grande que les nombre de test reelement voulu)
    uvec3 localThreadPos = gl_LocalInvocationID;
    ivec2 posToWrite = ivec2(localThreadPos.x/2, localThreadPos.y+1);
    vec4 value1 = imageLoad(imageMin, ivec2(localThreadPos.x-1, localThreadPos.y));
    vec4 value2 = imageLoad(imageMin, ivec2(localThreadPos.x, localThreadPos.y));
    uint res = uint(min(value1.r, value2.r));
    //uint res = uint(value1.r);
    imageAtomicExchange(imageMin, posToWrite, res);
}

    // Lecture de l'element de la texture que l'on veut
    //vec3 color= imageLoad(image, uint(gl_GlobalInvocationID.xy)).rgb;

    // shader
    /*int data[N];
    int tmp[N];
    uniform uint N;      // nombre de valeurs à traiter pour l'iteration 0, comparer les valeurs de data et ecrire dans tmp
    uniform uint n;     // nombre de valeurs à traiter pour les autres iterations, comparer les valeurs de tmp et les ecrire dans une partie libre de tmp
    layout(local_size= 1024)
    void main( )
    {
        uint id= GlobalID;

        if(id < N/2)
        // uniquement pour la premiere iteration
        tmp[N/2+id]= min(data[id*2], data[id*2+1]);

        if(id < n/2)
        {
            // pour les autres iterations
            tmp[n/2+id]= min(tmp[n+id*2], tmp[n+id*2+1]);
        }
    }*/

    #endif