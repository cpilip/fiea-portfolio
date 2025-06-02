#version 410 core
uniform vec3 col;
uniform vec3 ks;
uniform vec3 lightPos;
uniform float shininess;
uniform vec3 lightColor;
uniform bool enableLighting;

uniform sampler2D depthMap;
uniform float sigma;

in vec3 positionForFP;
in vec3 normalForFP;
in vec4 positionLightCVV;

out vec4 fragColor;

void main(void) {
	vec3 kd = vec3( col.x/2.0, col.y/2.0, col.z/2.0 );
	float shininess = 32.0;
	vec3 rgb = kd;

	if ( enableLighting ) {
		vec3 lightDirection = normalize( lightPos - positionForFP );
		float diffuse = max( dot( normalForFP, lightDirection), 0 );
		vec3 viewDirection = normalize( - positionForFP );
		vec3 halfVector = normalize( lightDirection + viewDirection );
		float specular = max( 0, dot( normalForFP, halfVector ) );
		if (diffuse == 0.0) {
		    specular = 0.0;
		} else {
	   		specular = pow( specular, shininess );
		}
		vec3 scatteredLight = kd * lightColor * diffuse;
		vec3 reflectedLight = ks * lightColor * specular;
		vec3 ambientLight = kd * vec3( 0.1, 0.1, 0.1 );

	    float shadow = 1.0; 

		// Shadow mapped lighting
		vec3 shadowTexCoord = positionLightCVV.xyz / positionLightCVV.w;
		shadowTexCoord = shadowTexCoord * 0.5 + 0.5; 

		/* Referencing explanations on sampler2DShadow suggests that there is slightly more setup to use 
		   sampler2DShadow correctly:
				- set GL_LINEAR instead of GL_NEAREST so that the sampler can sample more depth values
				- set GL_TEXTURE_COMPARE_MODE = GL_COMPARE_REF_TO_TEXTURE
				- get pcfDepth = texture(depthMap, vec3(shadowTexCoord.xy, shadowTexCoord.z - sigma)).r,
				which returns a value [0, 1] referencing an average of how many of the samples passed 
		
		   Ended up using sampler2D and manually taking 9 depth samples and averaging the result since that 
		   produced more visible results and required less tampering.
		*/ 

		shadow = 0.0;
		vec2 texOffset = 1.0 / textureSize(depthMap, 0); 

		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(depthMap, shadowTexCoord.xy + vec2(x, y) * texOffset).r; 
				
				if (shadowTexCoord.z - sigma <= pcfDepth)
				{
					shadow += 1.0;
				}
				else
				{
					shadow += 0.0;
				}       
			}    
		}
		
		shadow = shadow / 9.0;

	    rgb = min( shadow * (scatteredLight + reflectedLight) + ambientLight, vec3(1,1,1) );
 	} 
	fragColor = vec4( rgb ,1 );
}