@postprocess[body]
$src[^dom::set{$body}]
$dst[^src.xslt[global.xsl;$.param1(^default($form:param1;0))]]
$response:body[^dst.file[
	$.encoding[windows-1251]
	$.method[html]
]]

@default[val;deflt][v]
$v[$val]^if(def $v){$v}{$deflt}
