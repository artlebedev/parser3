<?xml version="1.0"?>
<!--
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">


<xsl:template match="blog-entry">
	<xsl:apply-templates />
</xsl:template>


<xsl:template match="cover">
	<div class="cover">
	<xsl:apply-templates />
	</div>
</xsl:template>

<xsl:template match="fullpage | full-page">
	<div class="fp">
	<xsl:apply-templates />
	</div>
</xsl:template>

<xsl:template match="text">
	<div>
	</div>
</xsl:template>

<xsl:template match="blog-entry">
	<xsl:apply-templates />
</xsl:template>


<xsl:template match="settings//*">
</xsl:template>

<xsl:template match="screen">
	
</xsl:template>

<xsl:template match="epigraph">
	<div class="epigraph">
		<xsl:apply-templates />
		<xsl:call-template name="author-of-quotation" />
	</div>
</xsl:template>

<xsl:template name="author-of-quotation">
<xsl:if test="@author">
	<p class="author">
	<xsl:value-of select="@author" /> 
	</p>
	</xsl:if>
</xsl:template>

<xsl:template match="image | img">
	<xsl:element name="img">
		<xsl:attribute name="src">
			<xsl:value-of select="@source | @src" />
		</xsl:attribute>

<!--
		<xsl:if test="@width | @height">
			<xsl:attribute name="@{name()}">
				<xsl:value-of select="@{name()}" />
			</xsl:attribute>
		</xsl:if>
-->
		<xsl:attribute name="width">
			<xsl:value-of select="@width" />
		</xsl:attribute>
		<xsl:attribute name="height">
			<xsl:value-of select="@height" />
		</xsl:attribute>

		<xsl:attribute name="alt">
			<xsl:value-of select="@alt" />
		</xsl:attribute>

		<xsl:if test="class">
			<xsl:attribute name="class">
				<xsl:value-of select="@class" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="style">
			<xsl:attribute name="style">
				<xsl:value-of select="@style" />
			</xsl:attribute>
		</xsl:if>


		<xsl:if test="@align">
			<xsl:attribute name="align">
				<xsl:value-of select="@align" />
			</xsl:attribute>
		</xsl:if>
	</xsl:element>
</xsl:template>


<xsl:template match="p | b | i | strong | em | sup | sub | small | nobr">
	<xsl:element name="{name()}">
		<xsl:apply-templates />
	</xsl:element>
</xsl:template>

</xsl:stylesheet>