<?xml version="1.0" encoding="windows-1251" ?>

<!DOCTYPE xsl:stylesheet SYSTEM "symbols.ent">

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	>
<!--method="html"-->

<xsl:output 
    omit-xml-declaration="no"
    encoding="windows-1251"
	/>
	
	<xsl:template match="/">
		<html>
		// 
		z
			<xsl:copy-of select="."/>
			<br/>
			<a href="str:encode-uri('������',1)">aaa</a>
		</html>
	</xsl:template>
	
	
</xsl:stylesheet>
