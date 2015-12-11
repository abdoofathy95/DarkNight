#define USE_INNER_AND_OUTER_CONES

uniform sampler2D colorMap;

#if defined(USE_INNER_AND_OUTER_CONES)
    uniform float cosOuterCone;
    uniform float cosInnerCone;
#endif

varying vec3 position;
varying vec3 normal;

void main()
{   
    vec3 l = gl_LightSource[0].position.xyz - position;
    float d = length(l);
    float atten = 1.0 / (gl_LightSource[0].constantAttenuation +
                         gl_LightSource[0].linearAttenuation * d +
                         gl_LightSource[0].quadraticAttenuation * d * d);
    
    l = normalize(l);

    float spotDot = dot(-l, normalize(gl_LightSource[0].spotDirection));

#if defined(USE_INNER_AND_OUTER_CONES)
    float spotEffect = smoothstep(cosOuterCone, cosInnerCone, spotDot);
#else    
    float spotEffect = (spotDot < gl_LightSource[0].spotCosCutoff)
                        ? 0.0 : pow(spotDot, gl_LightSource[0].spotExponent);
#endif

    atten *= spotEffect;

    vec3 n = normalize(normal);
    vec3 v = normalize(-position);
    vec3 h = normalize(l + v);
    
    float nDotL = max(0.0, dot(n, l));
    float nDotH = max(0.0, dot(n, h));
    float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);
    
    vec4 ambient = gl_FrontLightProduct[0].ambient * atten;
    vec4 diffuse = gl_FrontLightProduct[0].diffuse * nDotL * atten;
    vec4 specular = gl_FrontLightProduct[0].specular * power * atten;
    vec4 color = gl_FrontLightModelProduct.sceneColor + ambient + diffuse + specular;
    
    gl_FragColor = color;
}
