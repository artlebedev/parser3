# the tests of run_tests.sh: GET /?name calls @name

@main[]
^self.[$request:query][]

@fresh[]
^Counter:inc[] ^Counter:inc[]

@failed[]
^Cached:fail[]

@use_cached[]
^use[cached.p] ^Cached:hello[]

@extended[]
^use[extend.p] ^Cached:extra[]

@options[]
# a file compiled into a cached class takes the options of the class
^use[extend.p] ^Options:later[]locals ^Options:check[]

@defined[]
^reflection:class_alias[Counter;Late_defined] ^use[late.p]

@not_partial[]
^reflection:class_alias[Counter;Late_partial] ^use[late.p]

@no_base[]
^use[late.p] ^Late_derived:bye[]

@changing[]
^Changing:version[]
