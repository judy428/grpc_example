all:
	protoc ./protos/helloworld.proto --cpp_out=./dest/
	protoc --grpc_out=./dest/ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin ./protos/helloworld.proto
	#g++ -std=c++11 ./dest/test.pb.cc ./dest/test.grpc.pb.cc server.cc -o ./dest/server -L/usr/local/lib `pkg-config --cflags protobuf grpc` -std=c++11 `pkg-config --libs protobuf grpc++ grpc` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl

	# g++ -o client dest/test.pb.cc dest/test.grpc.pb.cc client.cpp -L/usr/local/mysoft/grpc-1.39.0/lib `pkg-config --cflags protobuf grpc` -std=c++11 `pkg-config --libs protobuf grpc++ grpc` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl

	g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc server.cc -o ./dest/server -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	/usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \
	
	g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc client.cc -o ./dest/client -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	/usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \

	 g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc asyncClient.cc -o ./dest/asyncClient -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	/usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \

	g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc server_async.cc -o ./dest/server_async -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	/usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \

	g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc server_async2.cc -o ./dest/server_async2 -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	/usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \

	 g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc server_async3.cc -o ./dest/server_async3 -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	/usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \

	g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc server_callback.cc -o ./dest/server_callback -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	/usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \

	 g++ -std=c++11 -g ./dest/protos/helloworld.pb.cc ./dest/protos/helloworld.grpc.pb.cc client_callback.cc -o ./dest/client_callback -I./dest/protos/ -I./dest/ -L/usr/local/lib -L/usr/local/lib64 -lpthread \
	 /usr/local/lib/libgrpc++_reflection.a            \
	 /usr/local/lib/libgrpc++.a                       \
	 /usr/local/lib64/libprotobuf.a                   \
	 /usr/local/lib/libgrpc.a                         \
	 /usr/local/lib/libz.a                            \
	 /usr/local/lib/libcares.a                        \
	 /usr/local/lib/libaddress_sorting.a              \
	 /usr/local/lib/libre2.a                          \
	 /usr/local/lib/libabsl_hash.a                    \
	 /usr/local/lib/libabsl_city.a                    \
	 /usr/local/lib/libabsl_wyhash.a                  \
	 /usr/local/lib/libabsl_raw_hash_set.a            \
	 /usr/local/lib/libabsl_hashtablez_sampler.a      \
	 /usr/local/lib/libabsl_exponential_biased.a      \
	 /usr/local/lib/libabsl_statusor.a                \
	 /usr/local/lib/libabsl_bad_variant_access.a      \
	 /usr/local/lib/libgpr.a                          \
	 /usr/local/lib/libupb.a                          \
	 /usr/local/lib/libabsl_status.a                  \
	 /usr/local/lib/libabsl_cord.a                    \
	 /usr/local/lib/libabsl_str_format_internal.a     \
	 /usr/local/lib/libabsl_synchronization.a         \
	 /usr/local/lib/libabsl_stacktrace.a              \
	 /usr/local/lib/libabsl_symbolize.a               \
	 /usr/local/lib/libabsl_debugging_internal.a      \
	 /usr/local/lib/libabsl_demangle_internal.a       \
	 /usr/local/lib/libabsl_graphcycles_internal.a    \
	 /usr/local/lib/libabsl_malloc_internal.a         \
	 /usr/local/lib/libabsl_time.a                    \
	 /usr/local/lib/libabsl_strings.a                 \
	 /usr/local/lib/libabsl_throw_delegate.a          \
	 /usr/local/lib/libabsl_strings_internal.a        \
	 /usr/local/lib/libabsl_base.a                    \
	 /usr/local/lib/libabsl_spinlock_wait.a           \
	 /usr/local/lib/libabsl_int128.a                  \
	 /usr/local/lib/libabsl_civil_time.a              \
	 /usr/local/lib/libabsl_time_zone.a               \
	 /usr/local/lib/libabsl_bad_optional_access.a     \
	 /usr/local/lib/libabsl_raw_logging_internal.a    \
	 /usr/local/lib/libabsl_log_severity.a            \
	 /usr/local/lib/libssl.a                          \
	 /usr/local/lib/libcrypto.a                       \

clean:
	rm -rf  ./dest/*