
#ifndef EAR_EXPORT_H
#define EAR_EXPORT_H

#ifdef EAR_STATIC_DEFINE
#  define EAR_EXPORT
#  define EAR_NO_EXPORT
#else
#  ifndef EAR_EXPORT
#    ifdef ear_EXPORTS
        /* We are building this library */
#      define EAR_EXPORT 
#    else
        /* We are using this library */
#      define EAR_EXPORT 
#    endif
#  endif

#  ifndef EAR_NO_EXPORT
#    define EAR_NO_EXPORT 
#  endif
#endif

#ifndef EAR_DEPRECATED
#  define EAR_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef EAR_DEPRECATED_EXPORT
#  define EAR_DEPRECATED_EXPORT EAR_EXPORT EAR_DEPRECATED
#endif

#ifndef EAR_DEPRECATED_NO_EXPORT
#  define EAR_DEPRECATED_NO_EXPORT EAR_NO_EXPORT EAR_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef EAR_NO_DEPRECATED
#    define EAR_NO_DEPRECATED
#  endif
#endif

#endif /* EAR_EXPORT_H */
