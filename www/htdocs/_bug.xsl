<?xml version="1.0" encoding="windows-1251" ?>

<!DOCTYPE xsl:stylesheet SYSTEM "symbols.ent">

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!--method="html"-->

<xsl:output 
    omit-xml-declaration="no"
	doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
	doctype-system="DTD/xhtml1-strict.dtd"
	/>
<!--	encoding="koi8-r" <<influences .save encoding-->

<!--<xsl:strip-space
	elements = "xsl:attribute" /> -->
<!--<xsl:strip-space elements="*"/>-->
	
	<xsl:template match="/doc">
		<html>
<!--		<head/>
		<xsl:value-of select="."/> - в шаблоне<br/>
		<тег атрибут="значение
		с переносом строки"/>
		<tag attribute_from_xml="{@attribute}"/>
		<br/>-->
		<tag2>
			<xsl:attribute name="attribute_created_by_xsl_attribute">
							value
			</xsl:attribute>
		</tag2>
<создать_тег>
	<xsl:attribute name="создать_атрибут">
		значение
	</xsl:attribute>
</создать_тег>
		</html>
	</xsl:template>
	
	
</xsl:stylesheet>
