# Versione minima richiesta di CMake
cmake_minimum_required(VERSION 3.10)

project(Boids)

# Imposta lo standard C++ (es. 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- 1. Trova e abilita OpenMP ---
find_package(OpenMP REQUIRED)

# --- 2. Trova le librerie SFML (MODIFICATO) ---
# I nomi dei componenti richiedono la maiuscola!
find_package(SFML 3.0 REQUIRED COMPONENTS
        Graphics
        Window
        System
)

# --- 3. Definisci il tuo programma ---
add_executable(BoidsSeq mainSeq.cpp Boid.cpp Boid.h)
add_executable(BoidsPar mainPar.cpp Boid.cpp Boid.h)
add_executable(BenchmarkS1 BenchmarkS1.cpp Boid.cpp Boid.h)
add_executable(BenchmarkS2 BenchmarkS2.cpp Boid.cpp Boid.h)
add_executable(BenchmarkS1S2 BenchmarkS1S2.cpp Boid.cpp Boid.h)
add_executable(BenchmarkSeq BenchmarkSeq.cpp Boid.cpp Boid.h)
add_executable(BenchmarkMain BenchmarkMain.cpp Boid.cpp Boid.h)
add_executable(BenchmarkS1S2Dynamic BenchmarkS1S2Dynamic.cpp Boid.cpp Boid.h)


# --- 4. Collega le librerie ---
target_link_libraries(BoidsSeq PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)
target_link_libraries(BoidsPar PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)
target_link_libraries(BenchmarkS1 PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)
target_link_libraries(BenchmarkS2 PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)
target_link_libraries(BenchmarkS1S2 PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)
target_link_libraries(BenchmarkSeq PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)
target_link_libraries(BenchmarkMain PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)
target_link_libraries(BenchmarkS1S2Dynamic PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)


