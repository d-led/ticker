_G.package.path=_G.package.path..[[;./?.lua;./?/?.lua]]
assert( require 'premake.quickstart' )

local OS=os.get()

make_solution 'ticker'

includedirs {
	'./RxCpp/Rx/v2/src'
}

make_console_app( 'rxcpp_example', './ticker/rxcpp_example.cpp' )
make_cpp11()
run_target_after_build()

if OS == 'windows' then
	make_console_app( 'ppl_example', './ticker/ppl_example.cpp' )
	make_cpp11()
end
