<?xml version="1.0" encoding="windows-1251" ?>

<!DOCTYPE xsl:stylesheet SYSTEM "symbols.ent">

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
          xmlns:math="http://exslt.org/math"
	>
<!--method="html"-->

<xsl:output 
	method="html"
    omit-xml-declaration="no"
    encoding="windows-1251"
	/>
	
	<xsl:template match="/">
		<html>
		<br/>
	<rnd><xsl:value-of select="math:random()"/></rnd>
	<sin><xsl:value-of select="math:sin(263)"/></sin>
		// 
		z
			<xsl:copy-of select="."/>
			<br/>
			<a href="str:encode-uri('москва',1)">aaa</a>
		</html>
	</xsl:template>
	
	
</xsl:stylesheet>