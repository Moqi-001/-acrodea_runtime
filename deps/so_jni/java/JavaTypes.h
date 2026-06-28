#ifndef SO_JAVA_TYPES_H
#define SO_JAVA_TYPES_H

typedef enum JavaType {
    JAVA_TYPE_UNKNOWN = 0,
    JAVA_TYPE_BOOLEAN = 1,  // Z
    JAVA_TYPE_BYTE    = 2,  // B
    JAVA_TYPE_CHAR    = 3,  // C
    JAVA_TYPE_SHORT   = 4,  // S
    JAVA_TYPE_INT     = 5,  // I
    JAVA_TYPE_LONG    = 6,  // J
    JAVA_TYPE_FLOAT   = 7,  // F
    JAVA_TYPE_DOUBLE  = 8,  // D
    JAVA_TYPE_OBJECT  = 9,  // L
} JavaType;

typedef void *JObject;
typedef void (*JObjectDtor)(JObject data);

typedef JObject JClass;
typedef JObject JArray;
typedef JObject JString;
typedef JObject JBuffer;

#ifdef DEBUG_SOLOADER
#define JAVA_BASE_CAST(_type, _obj, _magic) \
    ((_type)(((_obj) && (*(uint32_t *)(_obj) == _magic)) ? (_obj) : NULL))
#else
#define JAVA_BASE_CAST(_type, _obj, _magic) ((_type)(_obj))
#endif

#endif