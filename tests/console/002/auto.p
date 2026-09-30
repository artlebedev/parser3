# the config and the root auto.p at once; loaded twice with @conf it would throw "parser already configured"
@auto[filespec]
$loaded_as[^if(def $loaded_as){$loaded_as }$filespec]
