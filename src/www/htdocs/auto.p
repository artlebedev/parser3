@postprocess[body]
$src[^dom::set{$body}]
$dst[^src.xslt[global.xsl]]
$response:body[^dst.file[
	$.encoding[windows-1251]
	$.method[html]
]]
