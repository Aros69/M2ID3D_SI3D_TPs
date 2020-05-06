# This bash file is made for generating all ReadMe images, it will take a really long time so don't do it if it' unecessary
echo "Generating Images for the Readme, this process is very long (it took me 1h30) and will delete all images generated before." &&
echo "You have 10 seconds before the process starts and delete all (except the 4k pictures) existing pictures generated before" && 
sleep 10 &&
echo "Compiling : " && cmake . && make &&
echo "Removing all old images" && rm doc/readme/E* doc/readme/A* && 
echo "Image generation begin" && 
echo "Start time is $(date)" &&
startT=$(date +%s) &&


# Generate Ex2 pictures 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex2 -savePathAndFilename=doc/readme/Ex2_Cornell   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex2 -savePathAndFilename=doc/readme/Ex2_Geometry  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex2 -savePathAndFilename=doc/readme/Ex2_Emission  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex2 -savePathAndFilename=doc/readme/Ex2_Shadow    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex2 -savePathAndFilename=doc/readme/Ex2_Secondary -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 


# Generate Ex5 pictures
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Cornell   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Geometry  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Emission  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Shadow    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Secondary -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Cornell_1Ray   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Geometry_1Ray  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Emission_1Ray  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Shadow_1Ray    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Secondary_1Ray -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -trianglePointParametrization=squareRoot -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Cornell_1Ray_squareRoot   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -trianglePointParametrization=squareRoot -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Geometry_1Ray_squareRoot  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -trianglePointParametrization=squareRoot -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Emission_1Ray_squareRoot  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -trianglePointParametrization=squareRoot -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Shadow_1Ray_squareRoot    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -trianglePointParametrization=squareRoot -nbDirectRay=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Secondary_1Ray_squareRoot -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -pdf=CosAndDistance -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Cornell_cosPdf   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -pdf=CosAndDistance -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Geometry_cosPdf  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -pdf=CosAndDistance -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Emission_cosPdf  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -pdf=CosAndDistance -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Shadow_cosPdf    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -pdf=CosAndDistance -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Secondary_cosPdf -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -pdf=MIS -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Cornell_mis   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -pdf=MIS -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Geometry_mis  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -pdf=MIS -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Emission_mis  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -pdf=MIS -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Shadow_mis    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -pdf=MIS -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Secondary_mis -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -epsilon=0.00001 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Cornell_Epsilon00001   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -epsilon=0.00001 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Geometry_Epsilon00001  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -epsilon=0.00001 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Emission_Epsilon00001  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -epsilon=0.00001 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Shadow_Epsilon00001    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -epsilon=0.00001 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Secondary_Epsilon00001 -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -tonemapping=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Cornell_tonemapped   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -tonemapping=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Geometry_tonemapped  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -tonemapping=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Emission_tonemapped  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -tonemapping=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Shadow_tonemapped    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -tonemapping=1 -rayTraceTechnique=Ex5 -savePathAndFilename=doc/readme/Ex5_Secondary_tonemapped -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 


# Generate Ambiant Images
./bin/RayTraceCPU.exe -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Cornell_distrib   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Geometry_distrib  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Emission_distrib  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Shadow_distrib    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Secondary_distrib -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -directionParametrization=uniform -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Cornell_uniform   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Geometry_uniform  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Emission_uniform  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Shadow_uniform    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -rayTraceTechnique=Ambiant -savePathAndFilename=doc/readme/Ambiant_Secondary_uniform -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 


# Generate Ex7 Images
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Cornell_Ex5_noSave   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Geometry_Ex5_noSave  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Emission_Ex5_noSave  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Shadow_Ex5_noSave    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Secondary_Ex5_noSave -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -directLightSaveFile=doc/readme/Ex5_Cornell.taf    -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Cornell_Ex5_Save   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -directLightSaveFile=doc/readme/Ex5_Geometry.taf   -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Geometry_Ex5_Save  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -directLightSaveFile=doc/readme/Ex5_Emission.taf   -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Emission_Ex5_Save  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -directLightSaveFile=doc/readme/Ex5_Shadow.taf     -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Shadow_Ex5_Save    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -directLightSaveFile=doc/readme/Ex5_Secondary.taf  -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Secondary_Ex5_Save -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -directionParametrization=uniform -directLightSaveFile=doc/readme/Ex5_Cornell.taf    -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Cornell_Ex5_uniform   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -directLightSaveFile=doc/readme/Ex5_Geometry.taf   -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Geometry_Ex5_uniform  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -directLightSaveFile=doc/readme/Ex5_Emission.taf   -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Emission_Ex5_uniform  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -directLightSaveFile=doc/readme/Ex5_Shadow.taf     -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Shadow_Ex5_uniform    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -directionParametrization=uniform -directLightSaveFile=doc/readme/Ex5_Secondary.taf  -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Secondary_Ex5_uniform -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

./bin/RayTraceCPU.exe -tonemapping=1 -directLightSaveFile=doc/readme/Ex5_Cornell.taf    -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Cornell_Ex5_tonemapped   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt   && 
./bin/RayTraceCPU.exe -tonemapping=1 -directLightSaveFile=doc/readme/Ex5_Geometry.taf   -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Geometry_Ex5_tonemapped  -mesh=data/RayTracingData/geometry.obj  -orbiter=data/RayTracingData/geometryOrbiter.txt  && 
./bin/RayTraceCPU.exe -tonemapping=1 -directLightSaveFile=doc/readme/Ex5_Emission.taf   -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Emission_Ex5_tonemapped  -mesh=data/RayTracingData/emission.obj  -orbiter=data/RayTracingData/emissionOrbiter.txt  && 
./bin/RayTraceCPU.exe -tonemapping=1 -directLightSaveFile=doc/readme/Ex5_Shadow.taf     -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Shadow_Ex5_tonemapped    -mesh=data/RayTracingData/shadow.obj    -orbiter=data/RayTracingData/shadowOrbiter.txt    && 
./bin/RayTraceCPU.exe -tonemapping=1 -directLightSaveFile=doc/readme/Ex5_Secondary.taf  -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/Ex7_Secondary_Ex5_tonemapped -mesh=data/RayTracingData/secondary.obj -orbiter=data/RayTracingData/secondaryOrbiter.txt && 

# Generate Main pictures : 4k pictures : took 6h to generate
#./bin/RayTraceCPU.exe                   -imgWidth=3840 -imgHeight=2160 -nbDirectRay=1000 -nbIndirectRay=1000 -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/niceSecondPicture   -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt && 
#./bin/RayTraceCPU.exe -tonemapping=1    -imgWidth=3840 -imgHeight=2160 -nbDirectRay=1000 -nbIndirectRay=1000 -rayTraceTechnique=Ex7 -savePathAndFilename=doc/readme/niceFirstPicture    -mesh=data/RayTracingData/cornell.obj   -orbiter=data/RayTracingData/cornellOrbiter.txt && 

echo "Image generation end" &&
echo "End time is $(date)" &&
endT=$(date +%s) &&
echo "Generating all images took $(expr $endT - $startT)seconds thanks for you patience."