include 'premake'

make_solution 'ticker'

local OS = os.get()

includedirs {
	'./RxCpp/Rx/v2/src'
}

if OS == 'linux' then
	links 'pthread'
end

make_console_app( 'rxcpp_example', './ticker/rxcpp_example.cpp' )
make_cpp11()
run_target_after_build()

make_console_app( 'rxcpp_test', './ticker/rxcpp_test.cpp')
includedirs 'Catch/single_include'
make_cpp11()
run_target_after_build()

if OS == 'windows' then
	make_console_app( 'ppl_example', './ticker/ppl_example.cpp' )
	make_cpp11()
end
