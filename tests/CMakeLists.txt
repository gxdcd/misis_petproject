add_ctest(
    ctest_sample
    SOURCES "examples/ctest_sample.cpp")

add_doctest(
    doctest_sample
    SOURCES "examples/doctest_sample.cpp"
    LIBS utilities)