///////////////////////////////////////////////////////////////////////
#ifdef DEFERRED_GEOMETRY_PASS

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	float metallic;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out float metallicness;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = vec3(transpose(inverse(uWorldMatrix)) * vec4(aNormal, 1.0));
	metallicness = metallic;
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;
in float metallicness;

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform samplerCube irradianceMap;

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oNormals;
layout(location = 2) out vec4 oColor;

out float gl_FragDepth;

void main()
{
	vec3 c = texture(uTexture, vTexCoord).rgb;
	
	oPosition = vec4(vPosition, 1.0);
	oNormals = vec4(normalize(vNormal), 1.0);
	oColor = vec4(c*uColor, 1.0);

	vec3 I = normalize(vPosition - cameraPos);
    vec3 R = reflect(I, normalize(vNormal));
	vec4 ReflectionColor = vec4(texture(skybox, R).rgb, 1.0);

	vec3 ambient = texture(irradianceMap, vNormal).rgb;


    oColor = mix(vec4(uColor, 1.0), ReflectionColor, metallicness) * 1.7*vec4(ambient, 1.0);

	// * vec4(ambient, 1.0)

	gl_FragDepth = gl_FragCoord.z - 0.2;
}

#endif
#endif

///////////////////////////////////////////////////////////////////////
#ifdef DEFERRED_LIGHTING_PASS

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	float intensity;
	vec3 position;
	float radius;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[16];
};

out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;

	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	float intensity;
	vec3 position;
	float radius;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[16];
};

layout(location = 0) out vec4 oFinalRender;

uniform sampler2D uGPosition;
uniform sampler2D uGNormals;
uniform sampler2D uGDiffuse;

vec3 DirectionalLight(Light light, vec3 Normal, vec3 Diffuse)
{
	float cosAngle = max(dot(Normal, -light.direction), 0.0); 
    vec3 ambient = 0.1 * light.color;
    vec3 diffuse = 0.9 * light.color * cosAngle * light.intensity;

    return (ambient + diffuse) * Diffuse;
}

vec3 PointLight(Light light, vec3 FragPos, vec3 Normal)
{
	vec3 N = normalize(Normal);
	vec3 L = normalize(light.position - FragPos);

	float threshold = 1.0;

	float shadowIntensity = 1.0;

	float dist = distance(light.position, FragPos);

	if(dist > light.radius)
		shadowIntensity = 1.0 - ((dist - light.radius) / threshold);

	// Hardcoded specular parameter
    vec3 specularMat = vec3(1.0);


	// Specular
    float specularIntensity = pow(max(0.0, dot(N, L)), 1.0);
    vec3 specular = specularMat * specularIntensity;

	// Diffuse
    float diffuseIntensity = max(0.0, dot(N, L));

	return vec3(specular + diffuseIntensity) * shadowIntensity * light.intensity * light.color;
}

void main()
{
	vec3 FragPos = texture(uGPosition, vTexCoord).rgb;
    vec3 Normal = texture(uGNormals, vTexCoord).rgb;
    vec3 Diffuse = texture(uGDiffuse, vTexCoord).rgb;

	vec3 viewDir = normalize(uCameraPosition - FragPos);

	vec3 lighting = Diffuse * 1.0;
    for(int i = 0; i < uLightCount; ++i)
    {
		switch(uLight[i].type)
		{
			case 0: // Directional
			{
                lighting += DirectionalLight(uLight[i], Normal, Diffuse);
			}
			break;

			case 1: // Point
			{
				float distance = length(uLight[i].position - FragPos);
				if(distance < uLight[i].radius)
				{
					lighting += PointLight(uLight[i], FragPos, Normal);
				}
			}
			break;

			default:
			{
				
			}
			break;
		}
    }

	oFinalRender = vec4(lighting * Diffuse, 1.0);

}

#endif
#endif
// This comment prevents a shader compilation error. Shader compiler was adding non-ASCII characters in this line, and couldn't process it
///////////////////////////////////////////////////////////////////////
#ifdef TEXTURED_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;

uniform sampler2D uTexture;

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = texture(uTexture, vTexCoord);
}

#endif
#endif
// This comment prevents a shader compilation error. Shader compiler was adding non-ASCII characters in this line, and couldn't process it