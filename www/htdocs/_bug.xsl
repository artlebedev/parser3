<?xml version="1.0" encoding="windows-1251" ?>

<!DOCTYPE xsl:stylesheet SYSTEM "symbols.ent">

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output 
	method="html"
	doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
	doctype-system="DTD/xhtml1-strict.dtd"
	/>
<!--	encoding="koi8-r" <<influences .save encoding-->
	
	<xsl:template match="/doc">
		<html>
		<head/>
		<xsl:value-of select="."/> - в шаблоне<br/>
		<тег атрибут="значение"/>
		</html>
	</xsl:template>
	
	
</xsl:stylesheet>
