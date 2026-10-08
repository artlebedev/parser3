/* specified manually on Windows [automaticaly set on Unix] */
#ifndef PARSER_VERSION
#ifdef _WIN64
#define PARSER_VERSION "3.5.2b (compiled on amd64-pc-win64)"
#else
#define PARSER_VERSION "3.5.2b (compiled on i386-pc-win32)"
#endif
#endif

// what the build includes, shown after the version
#ifdef PA_SAFE_MODE
#	define PA_FEATURE_SAFE_MODE " safe-mode"
#else
#	define PA_FEATURE_SAFE_MODE ""
#endif
#ifdef XML
#	define PA_FEATURE_XML " xml"
#else
#	define PA_FEATURE_XML ""
#endif
#ifdef WITH_AMQP
#	ifdef WITH_AMQP_SSL
#		define PA_FEATURE_AMQP " amqp+ssl"
#	else
#		define PA_FEATURE_AMQP " amqp"
#	endif
#else
#	define PA_FEATURE_AMQP ""
#endif
#ifdef WITH_REDIS
#	ifdef WITH_REDIS_SSL
#		define PA_FEATURE_REDIS " redis+ssl"
#	else
#		define PA_FEATURE_REDIS " redis"
#	endif
#else
#	define PA_FEATURE_REDIS ""
#endif
#ifdef WITH_MAILRECEIVE
#	define PA_FEATURE_MAILRECEIVE " mail"
#else
#	define PA_FEATURE_MAILRECEIVE ""
#endif

#define PARSER_VERSION_WITH_FEATURES PARSER_VERSION PA_FEATURE_SAFE_MODE PA_FEATURE_XML PA_FEATURE_AMQP PA_FEATURE_REDIS PA_FEATURE_MAILRECEIVE
