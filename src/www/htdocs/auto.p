@postprocess[body]
$src[^dom::set{$body}]
$dst[^src.xslt[global.xsl;$.param1[$form:param1]]]
$response:body[^dst.file[
	$.encoding[windows-1251]
	$.method[html]
]]