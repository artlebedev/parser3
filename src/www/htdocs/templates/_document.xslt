<?xml version="1.0" encoding="windows-1251" ?>

<!DOCTYPE xsl:stylesheet SYSTEM "http://localhost/templates/symbols.ent">

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="xml" omit-xml-declaration="no" indent="no" />
	<!--xsl:output method="xml" omit-xml-declaration="no" encoding="windows-1251" indent="no" /-->

	<xsl:template match="document">
		<document><xsl:apply-templates select="block | map | h1 | h2 | h3 | form | img | table | roaming-city-operators | tariffs-short | tariffs-long | phone | form-sendsms | script"/></document>
	</xsl:template>
	
	<xsl:template match="script">
		<script><xsl:comment>
			<xsl:value-of select="." />
		//</xsl:comment>
		</script>
	</xsl:template>
	
	<xsl:template match="block[attribute::type='comment']">
		<span style="font-size:smaller">
			<p style="margin-bottom:0"><b>Примечание</b></p>
			<xsl:apply-templates select="p | ul | ol"/>
		</span>
	</xsl:template>
	
	<xsl:template match="block[attribute::type='attention']">
		<span style="font-size:smaller">
			<p style="margin-bottom:0"><b>Внимание!</b></p>
			<xsl:apply-templates select="p | ul | ol "/>
		</span>
	</xsl:template>
	
	<xsl:template match="span[attribute::type='comment']">
		<span style="font-size:smaller">
			<b>Примечание:</b> <xsl:apply-templates select="text()"/>
		</span>
	</xsl:template>
	
	<xsl:template match="block[attribute::type='comment']/p | block[attribute::type='comment']/ul | block[attribute::type='comment']/ol | block[attribute::type='attention']/p | block[attribute::type='attention']/ul | block[attribute::type='attention']/ol">
		<xsl:element name="{name()}" namespace="">
			<xsl:if test="position()=1"><xsl:attribute name="style">margin-top:0</xsl:attribute></xsl:if>
			<xsl:apply-templates />
		</xsl:element>
		<xsl:apply-templates select="p | ul | ol "/>
	</xsl:template>
	
	<xsl:template match="block">
		<xsl:apply-templates select="p | img | ul | ol"/>
	</xsl:template>
	
	<xsl:template match="sup">
		<sup><nobr><font color="#ff0000" size="-1"><xsl:apply-templates/></font></nobr></sup>
	</xsl:template>

	
	<xsl:template match="a">
		<a>
			<xsl:if test="string-length(@href)!=0"><xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@name)!=0"><xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@class)!=0"><xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute></xsl:if>
			<xsl:apply-templates/>
		</a>
	</xsl:template>
	
	<xsl:template match="img">
		<img>
			<xsl:attribute name="src"><xsl:value-of select="@src"/></xsl:attribute>
			<xsl:attribute name="alt"><xsl:value-of select="@alt"/></xsl:attribute>
			<xsl:attribute name="title"><xsl:value-of select="@title"/></xsl:attribute>
			<xsl:if test="string-length(@width)!=0"><xsl:attribute name="width"><xsl:value-of select="@width"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@height)!=0"><xsl:attribute name="height"><xsl:value-of select="@height"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@align)!=0"><xsl:attribute name="align"><xsl:value-of select="@align"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@border)!=0"><xsl:attribute name="border"><xsl:value-of select="@border"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@usemap)!=0"><xsl:attribute name="usemap"><xsl:value-of select="@usemap"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@vspace)!=0"><xsl:attribute name="vspace"><xsl:value-of select="@vspace"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@hspace)!=0"><xsl:attribute name="hspace"><xsl:value-of select="@hspace"/></xsl:attribute></xsl:if>
		</img>
	</xsl:template>
	
	
	<xsl:template match="acrobat">
	<p>Для просмотра материалов Вам потребуется <a href="http://www.adobe.com/products/acrobat/readermain.html">Acrobat Reader</a></p>
	</xsl:template>
	
	
	<xsl:template match="font">
		<font size="{@size}" color="{@color}"><xsl:apply-templates/></font>
	</xsl:template>
	
	
	<xsl:template match="ol | ul">
		<xsl:element name="{name()}" namespace="">
			<xsl:if test="string-length(@class)!=0"><xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute></xsl:if>
			<xsl:if test="@type"><xsl:attribute name="type"><xsl:value-of select="@type"/></xsl:attribute></xsl:if>
			<xsl:apply-templates select="li"/>
		</xsl:element>
	</xsl:template>
	
	
	<xsl:template match="hicolor">
		<font color="#ce0029"><b><xsl:apply-templates/></b></font>
	</xsl:template>
	
	
	<xsl:template match="br">
	   <xsl:choose>
			<xsl:when test="@clear">
				<br clear="{@clear}" />
			</xsl:when>
			<xsl:otherwise>
				<br />
			</xsl:otherwise>
	   </xsl:choose>
	</xsl:template>
	
	<xsl:template match="hr">
		<hr size="1" noshade="noshade"/>
	</xsl:template>
	
	
	<xsl:template match="li | nobr | center | strike | b | i | p | pre | h1 | h2 | h3">
		<xsl:element name="{name()}" namespace="">
			<xsl:if test="string-length(@class)!=0"><xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute></xsl:if>
			<xsl:apply-templates select="p | nobr | center | strike | b | i | a | img | br | span | ul | ol | sup | input | textarea | select | text()" />
		</xsl:element>
	</xsl:template>

	
	<xsl:template match="map">
		<map name="{@name}">
			<xsl:for-each select="area"><area alt="{@alt}" href="{@href}" coords="{@coords}" shape="{@shape}" /></xsl:for-each>
		</map>
	</xsl:template>

	<!--===================================================-->
	<!--== ЭЛЕМЕНТЫ ФоРМЫ =================================-->
	<!--===================================================-->
	
	<xsl:template match="form">
		<form method="{@method}" action="{@action}" name="{@name}" onsubmit="{@onsubmit}">
			<xsl:apply-templates select="input | field | select | p | br | text()" />
		</form>
	</xsl:template>

	<xsl:template match="select">
		<select name="{@name}">
			<xsl:for-each select="option"><option value="{@value}">
				<xsl:if test="@selected='true'"><xsl:attribute name="selected">true</xsl:attribute></xsl:if>
				<xsl:value-of select="."/></option>
			</xsl:for-each>
		</select>
	</xsl:template>

	<xsl:template match="input">
		<xsl:choose>
			<xsl:when test="@type='radio' or @type='checkbox'">
				<xsl:if test="@type='radio'">
					<input type="{@type}" name="{@name}" value="{@value}" onclick="{@onclick}" id="{@id}">
						<xsl:if test="@checked"><xsl:attribute name="checked">true</xsl:attribute></xsl:if>
						<label for="{@id}"><xsl:value-of select="." /></label>
					</input><br />					
				</xsl:if>
				<xsl:if test="@type='checkbox'">
					<xsl:if test="@padding='left'">&nbsp;&nbsp;&nbsp;</xsl:if>
					<input type="{@type}" name="{@name}" value="{@value}" onclick="{@onclick}" id="{@id}"><label for="{@id}"><xsl:value-of select="." /></label></input><br />
				</xsl:if>
			</xsl:when>
			<xsl:otherwise>
				<input type="{@type}" name="{@name}" value="{@value}" onclick="{@onclick}" class="{@class}" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template match="textarea">
		<textarea name="{@name}" onclick="{@onclick}" cols="{@cols}" rows="{@rows}" class="{@class}" >
		</textarea>
	</xsl:template>

	<xsl:template match="form/field">
		<p>
			<xsl:if test="string-length(@title)">
				<font size="-1"><xsl:value-of select="@title" /></font>
				<xsl:if test="string-length(@note)">
					<font size="-1" color="red"> <xsl:value-of select="@note" /></font>
				</xsl:if>
				<br />
			</xsl:if>
			<xsl:apply-templates select="select | input | textarea | div" />
			
		</p>
	</xsl:template>

	  <xsl:template match="field/div">
		<div style="{@style}">
			<xsl:apply-templates select="input" />
		</div>
	</xsl:template>



	<!--===================================================-->
	<!--== ТАБЛИЦЫ ========================================-->
	<!--===================================================-->

	<xsl:template match="table">
		<table border="0" cellpadding="2" cellspacing="1" width="100%">
			<xsl:if test="@align"><xsl:attribute name="align"><xsl:value-of select="@align"/></xsl:attribute></xsl:if>
			<xsl:if test="@cellspacing"><xsl:attribute name="cellspacing"><xsl:value-of select="@cellspacing"/></xsl:attribute></xsl:if>
			<xsl:if test="@cellpadding"><xsl:attribute name="cellpadding"><xsl:value-of select="@cellpadding"/></xsl:attribute></xsl:if>
			<xsl:if test="@width"><xsl:attribute name="width"><xsl:value-of select="@width"/></xsl:attribute></xsl:if>
			<xsl:if test="@height"><xsl:attribute name="height"><xsl:value-of select="@height"/></xsl:attribute></xsl:if>
			<xsl:if test="@background"><xsl:attribute name="background"><xsl:value-of select="@background"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@class)!=0"><xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute></xsl:if>
			<xsl:apply-templates select="tr"/>
		</table>
	</xsl:template>
	
	<xsl:template match="tr">
		<tr valign="top">
			<xsl:if test="@class"><xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute></xsl:if>
			<xsl:choose>
				<xsl:when test="not(position() mod 2)"><xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute></xsl:when>
				<xsl:otherwise><xsl:attribute name="bgcolor">#cfd7e5</xsl:attribute></xsl:otherwise>
			</xsl:choose>
			<xsl:if test="@background">
				<xsl:choose>
					<xsl:when test="@background='null'"><xsl:attribute name="background"></xsl:attribute></xsl:when>
					<xsl:otherwise><xsl:attribute name="background"><xsl:value-of select="@background"/></xsl:attribute></xsl:otherwise>
				</xsl:choose>
			</xsl:if>
			<xsl:if test="@bgcolor">
				<xsl:choose>
					<xsl:when test="@bgcolor='null'"><xsl:attribute name="bgcolor"></xsl:attribute></xsl:when>
					<xsl:otherwise><xsl:attribute name="bgcolor"><xsl:value-of select="@bgcolor"/></xsl:attribute></xsl:otherwise>
				</xsl:choose>
			</xsl:if>
			<xsl:if test="@valign"><xsl:attribute name="valign"><xsl:value-of select="@valign"/></xsl:attribute></xsl:if>
			<xsl:if test="@align"><xsl:attribute name="align"><xsl:value-of select="@align"/></xsl:attribute></xsl:if>
			<xsl:apply-templates select="td"/>
		</tr>
	</xsl:template>

	<xsl:template match="td">
		<td align="left">
			<xsl:if test="@colspan"><xsl:attribute name="colspan"><xsl:value-of select="@colspan"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@style)!=0"><xsl:attribute name="style"><xsl:value-of select="@style"/></xsl:attribute></xsl:if>
			<xsl:if test="string-length(@class)!=0"><xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute></xsl:if>
			<xsl:if test="@background">
				<xsl:choose>
					<xsl:when test="@background='null'"><xsl:attribute name="background"></xsl:attribute></xsl:when>
					<xsl:otherwise><xsl:attribute name="background"><xsl:value-of select="@background"/></xsl:attribute></xsl:otherwise>
				</xsl:choose>
			</xsl:if>
			<xsl:if test="@bgcolor">
				<xsl:choose>
					<xsl:when test="@bgcolor='null'"><xsl:attribute name="bgcolor"></xsl:attribute></xsl:when>
					<xsl:otherwise><xsl:attribute name="bgcolor"><xsl:value-of select="@bgcolor"/></xsl:attribute></xsl:otherwise>
				</xsl:choose>
			</xsl:if>
			<xsl:if test="@rowspan"><xsl:attribute name="rowspan"><xsl:value-of select="@rowspan"/></xsl:attribute></xsl:if>
			<xsl:if test="@valign"><xsl:attribute name="valign"><xsl:value-of select="@valign"/></xsl:attribute></xsl:if>
			<xsl:if test="@align"><xsl:attribute name="align"><xsl:value-of select="@align"/></xsl:attribute></xsl:if>
			<xsl:if test="@width"><xsl:attribute name="width"><xsl:value-of select="@width"/></xsl:attribute></xsl:if>
			<xsl:if test="@height"><xsl:attribute name="height"><xsl:value-of select="@height"/></xsl:attribute></xsl:if>
			<xsl:if test="@nowrap"><xsl:attribute name="nowrap"><xsl:value-of select="@nowrap"/></xsl:attribute></xsl:if>
			<xsl:choose>
				<xsl:when test="@header='header'"><xsl:attribute name="bgcolor">#ffffff</xsl:attribute></xsl:when>
				<xsl:otherwise></xsl:otherwise>
			</xsl:choose>
			<font size="2"><xsl:apply-templates/></font>
		</td>
	</xsl:template>


	<!--===================================================-->
	<!--== РОУМИНГ ========================================-->
	<!--===================================================-->

	<xsl:template match="operator_info">
		<tr>
			 <td width="16"><font size="-1"><a href="/coverage/i_roaming/operator_file/{@filename}.{@type}"><img src="{@img}"  width="16" height="16" border="0"/></a></font></td>
			 <td><font size="-1"><a href="/coverage/i_roaming/operator_file/{@filename}.{@type}"><xsl:value-of select="@name"/></a> (<xsl:value-of select="@size"/>)</font></td>
		 </tr>
		<xsl:apply-templates/>	
	</xsl:template>

	<xsl:template match="block[attribute::type='notes']">
		<table border="0" cellpadding="0" cellspacing="0">
			<xsl:for-each select="item">
				<tr valign="top">
					<td width="10"><sup><nobr><font color="#ff0000" size="-1"><xsl:value-of select="sup"/></font></nobr></sup></td>
					<td><font size="-1"><xsl:apply-templates select="p|ul"/></font></td>
				</tr>
				<tr>
					<td colspan="2"><br/></td>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:template>

	<xsl:template match="roaming-city-operators">
			
			<table cellpadding="10" width="100%" border="0" background="">
				<tr valign="top">
					<td width="24%"><br /></td>
					<td width="76%"><xsl:apply-templates select="h1"/></td>
				</tr>
				<tr valign="top">
					<td width="24%"><br /></td>
					<td width="76%"><br /><xsl:apply-templates select="form"/></td>
				</tr>
				<tr valign="top">
					<td width="24%"><font color="#2f405a" size="-1"><xsl:apply-templates select="page-quote"/></font></td>
					<td width="76%"><xsl:apply-templates select="block/p"/></td>
				</tr>
			</table>
			<table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
			 <br/>
			 <table border="0" cellpadding="4" cellspacing="1" width="25%" background="">
				 <tr>
					 <td bgcolor="#ffb700" align="center" class="title">ОПЕРАТОРЫ</td>
				 </tr>
				 </table><br/>
					 <table width="100%" border="0" cellspacing="1" cellpadding="2" background="">
					 <tr valign="top">
						 <td width="25%"><font size="-1">Компания</font></td>
						 <td width="25%"><font size="-1">Контактная информация</font></td>
						 <td width="50%"><font size="-1">Зона обслуживания</font></td>
					 </tr>
					 <xsl:for-each select="operator">
						 <tr valign="top">
							 <xsl:choose>
								 <xsl:when test="position() mod 2">
									 <xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute>
								 </xsl:when>
								 <xsl:otherwise>
									 <xsl:attribute name="bgcolor">#cfd7e5</xsl:attribute>
								 </xsl:otherwise>
							 </xsl:choose>
							 <td width="25%">
								 <xsl:choose>
									 <xsl:when test="name_coord_oper!=''"><font size="-1">
										 Региональный оператор:</font><br/><b><xsl:value-of select="name"/></b><br/><br/><font size="-1">
										 Координирующий оператор:<br/><b><xsl:value-of select="name_coord_oper"/></b><br/><br/></font>
									 </xsl:when>
									 <xsl:otherwise>
										 <b><xsl:value-of select="name"/></b><br/><br/>
									 </xsl:otherwise>
								 </xsl:choose>
								<font size="-1">
								Диапазон:<br/><b>GSM-<xsl:value-of select="range"/></b><xsl:apply-templates select="range_footnote"/><br/><br/>
								Название сети:<br/><b><xsl:value-of select="netname"/></b><br/><br/>
								 Мобильный код сети:<br/><b><xsl:value-of select="netcode"/></b>
								 </font>
							 </td>
							 <td width="25%">
								 <font size="-1">
								 <xsl:if test="address!=''">
									 <xsl:apply-templates select="address"/><br/><br/>
								 </xsl:if>
								 <xsl:if test="hours!=''">
									 Время работы:<br/><xsl:value-of select="hours"/><br/><br/>
								 </xsl:if>
								 <xsl:if test="phones!=''">
									 Телефоны:<br/><xsl:apply-templates select="phones"/><br/><br/>
								 </xsl:if>
								 <xsl:if test="telefax!=''">
									 Телефакс:<br/><xsl:value-of select="telefax"/><br/><br/>
								 </xsl:if>
								 <xsl:if test="email!=''">
									 <nobr>E-mail</nobr>:<br/><a href="mailto:{email}"><xsl:value-of select="email"/></a><br/><br/>
								 </xsl:if>
								 <xsl:if test="www!=''">
									 Адрес в&#160;Интернете:<br/><a href="http://{www}{wwwslash}"><xsl:value-of select="www"/></a><br/>
								 </xsl:if>
								 <xsl:if test="operator_info[@name!='']">
									 <table border="0" cellpadding="2" cellspacing="0">
									 <tr>
									 <td colspan="2"><font size="-1">Информация:</font></td>
									 </tr>
									 <xsl:apply-templates select="operator_info"/><br/>
									 </table>
								 </xsl:if>
								 <br/>
								</font>
							 </td>
							 <td width="50%">
								 <xsl:choose>
									 <xsl:when test="azone='siktivkar'">
										 <font size="-1"><a href="/coverage/komi/{azone}.html"><xsl:value-of select="zone"/></a></font>
									 </xsl:when>
									 <xsl:when test="(azone!='') and (azone!='siktivkar')">
										 <font size="-1"><a href="/coverage/{azone}/"><xsl:value-of select="zone"/></a></font>
									 </xsl:when>
									 <xsl:otherwise>
										 <font size="-1"><xsl:apply-templates select="zone"/></font>
									 </xsl:otherwise>
								 </xsl:choose><br/>
							 </td>
						 </tr>
					 </xsl:for-each>
					 </table><br/>
					 <table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
					 <br/><table border="0" cellpadding="4" cellspacing="1" width="25%" background="">
					 <tr>
						 <td bgcolor="#ffb700" align="center" class="title">ВХОДЯЩИЕ / ИСХОДЯЩИЕ ВЫЗОВЫ</td>
					 </tr>
					 </table><br/>
					 <xsl:choose>
						 <xsl:when test="operator/vt1/value!=''">
							 <table width="100%" border="0" cellspacing="1" cellpadding="2" background="">
							 <tr valign="top">
								 <td width="25%"><font size="-1"><br/></font></td>
								 <td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/vt1/title"/></font></td>
								 <td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/vt2/title"/></font></td>
								 <td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/vt5/title"/></font></td>
							 </tr>
							 <xsl:for-each select="operator">
								 <tr valign="top">
									 <xsl:choose>
										 <xsl:when test="position() mod 2">
											 <xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute>
										 </xsl:when>
										 <xsl:otherwise>
											 <xsl:attribute name="bgcolor">#cfd7e5</xsl:attribute>
										 </xsl:otherwise>
									 </xsl:choose>
									 <xsl:choose>
										 <xsl:when test="vt2/value='oblast'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к тарифной зоне &#171;<a href="/coverage/tariff_zone.html">Область</a>&#187;.<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										 <xsl:when test="vt2/value='ln_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне внутрисетевого роуминга (<xsl:value-of select="name"/>).<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/inroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
																				 <xsl:when test="vt2/value='l_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне льготного национального роуминга. Тарифы смотрите в разделе &#171;<a href="/tariffs/nroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										 <xsl:otherwise>
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td><font size="-1"><xsl:value-of select="vt1/value"/> <xsl:apply-templates select="vt1/footnote"/></font></td>
											 <td><font size="-1"><xsl:value-of select="vt2/value"/> <xsl:apply-templates select="vt2/footnote"/></font></td>
											 <td><font size="-1"><xsl:value-of select="vt5/value"/> <xsl:apply-templates select="vt5/footnote"/></font></td>
										 </xsl:otherwise>
									 </xsl:choose>
								 </tr>
							 </xsl:for-each>

							 <tr valign="top">
								 <td width="25%"><font size="-1"><br/></font></td>
								<td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/group1/title"/></font></td>
								<td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/group2/title"/></font></td>
								 <td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/group3/title"/></font></td>
							 </tr>
							 <xsl:for-each select="operator">
								 <tr valign="top">
									 <xsl:choose>
										 <xsl:when test="position() mod 2">
											 <xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute>
										 </xsl:when>
										 <xsl:otherwise>
											 <xsl:attribute name="bgcolor">#cfd7e5</xsl:attribute>
										 </xsl:otherwise>
									 </xsl:choose>
									 <xsl:choose>
										 <xsl:when test="vt2/value='oblast'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к тарифной зоне &#171;<a href="/coverage/tariff_zone.html">Область</a>&#187;.<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										 <xsl:when test="vt2/value='ln_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне внутрисетевого роуминга (<xsl:value-of select="name"/>).<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/inroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										<xsl:when test="vt2/value='l_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне льготного национального роуминга. Тарифы смотрите в разделе &#171;<a href="/tariffs/nroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
									<xsl:otherwise>
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td><font size="-1"><xsl:value-of select="group1/value"/></font></td>
											 <td><font size="-1"><xsl:value-of select="group2/value"/></font></td>
											<td><font size="-1"><xsl:value-of select="group3/value"/></font></td>
									 </xsl:otherwise>
									 </xsl:choose>
								 </tr>
							 </xsl:for-each>

							 <tr valign="top">
								 <td width="25%"><font size="-1"><br/></font></td>
								 <td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/vt6/title"/></font></td>
								 <td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/vt8/title"/></font></td>
								 <td width="25%"><font size="-1"><xsl:apply-templates select="operator[position()=1]/vt7/title"/></font></td>
							 </tr>
							 <xsl:for-each select="operator">
								 <tr valign="top">
									 <xsl:choose>
										 <xsl:when test="position() mod 2">
											 <xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute>
										 </xsl:when>
										 <xsl:otherwise>
											 <xsl:attribute name="bgcolor">#cfd7e5</xsl:attribute>
										 </xsl:otherwise>
									 </xsl:choose>
									 <xsl:choose>
										 <xsl:when test="vt2/value='oblast'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к тарифной зоне &#171;<a href="/coverage/tariff_zone.html">Область</a>&#187;.<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										 <xsl:when test="vt2/value='ln_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне внутрисетевого роуминга (<xsl:value-of select="name"/>).<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/inroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
																				<xsl:when test="vt2/value='l_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне льготного национального роуминга. Тарифы смотрите в разделе &#171;<a href="/tariffs/nroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
																				<xsl:otherwise>
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td><font size="-1"><xsl:value-of select="vt6/value"/> <xsl:apply-templates select="vt6/footnote"/></font></td>
											 <td><font size="-1"><xsl:value-of select="vt8/value"/> <xsl:apply-templates select="vt8/footnote"/></font></td>
											 <td><font size="-1"><xsl:value-of select="vt7/value"/> <xsl:apply-templates select="vt7/footnote"/></font></td>
										 </xsl:otherwise>
									 </xsl:choose>
								 </tr>
							 </xsl:for-each>
							 </table><br/>
						 </xsl:when>
						 <xsl:otherwise>
							 <table width="100%" border="0" cellspacing="1" cellpadding="2" background="">
							 <xsl:for-each select="operator">
								 <tr valign="top">
									 <xsl:choose>
										 <xsl:when test="position() mod 2">
											 <xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute>
										 </xsl:when>
										 <xsl:otherwise>
											 <xsl:attribute name="bgcolor">#cfd7e5</xsl:attribute>
										 </xsl:otherwise>
									 </xsl:choose>
									 <xsl:choose>
										 <xsl:when test="vt2/value='oblast'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к тарифной зоне &#171;<a href="/coverage/tariff_zone.html">Область</a>&#187;.<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										 <xsl:when test="vt2/value='ln_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне внутрисетевого роуминга (<xsl:value-of select="name"/>).<br/>Тарифы смотрите в разделе &#171;<a href="/tariffs/inroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										 <xsl:when test="vt2/value='l_roaming'">
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Город относится к зоне льготного национального роуминга. Тарифы смотрите в разделе &#171;<a href="/tariffs/nroaming.html">Тарифы</a>&#187; соответственно Вашему тарифному плану.</font></td>
										 </xsl:when>
										 <xsl:otherwise>
											 <td><font size="-1"><b><xsl:value-of select="name"/></b></font></td>
											 <td colspan="3"><font size="-1">Тарифы отсутствуют в базе данных.</font></td>
										 </xsl:otherwise>
									 </xsl:choose>
								 </tr>
							 </xsl:for-each>
							 </table><br/>
						 </xsl:otherwise>
					 </xsl:choose>
			
					<table cellpadding="10" width="100%" border="0" background="">
					<tr valign="top">
						<td width="24%"><br /></td>
						<td width="76%"><xsl:apply-templates select="block[attribute::type='notes']"/></td>
					</tr>
					</table>
		
	</xsl:template>

	<!--===================================================-->
	<!--== ТАРИФЫ =========================================-->
	<!--===================================================-->

	<xsl:template match="tariffs-short">
		
				<tr valign="top" bgcolor="#ffffff">
				
					<td width="20%" colspan="2" class="small" rowspan="3">
						<xsl:apply-templates select="phoneNavigationColumn" />
					</td>
				
					<td width="20%" colspan="2" class="td-border">
						<table border="0" cellspacing="0" cellpadding="0" width="100%" background="/off-line/tariffs/tariff.html/{@id}_line.gif" style="background-repeat: repeat-x">
						<tr>
							<td><img src="/off-line/tariffs/tariff.html/{@id}_background1.gif" width="{@width1}" height="{@height1}"/></td>
							<td align="right"><img src="/off-line/tariffs/tariff.html/{@id}_background2.gif" width="{@width2}" height="{@height2}"/></td>
						</tr>
						</table>
						<table border="0" cellspacing="0" cellpadding="0" width="100%">
						<tr>
							<td valign="middle" align="right" colspan="2">
							<table border="0" cellspacing="0" cellpadding="5" background=""><tr><td align="right">
								<font size="-1">тариф</font><br/>
								<b><font size="+1"><xsl:value-of select="@name"/></font></b><br /><br />
								<xsl:choose>
									<xsl:when test="@archive">
										<a href="./tariff_complete.html?id={@id}&amp;archive=true"><font size="-1">подробнее</font></a><br />
									</xsl:when>
									<xsl:otherwise>
										<a href="./tariff_complete.html?id={@id}"><font size="-1">подробнее</font></a><br />
									</xsl:otherwise>
								</xsl:choose>	
								<xsl:if test="@pdfversion">
									<br />
									<img src="/i/icons/pdf.gif" width="16" height="16" border="0" align="absmiddle"/><font size="-1">&nbsp;&nbsp;</font><a href="{@pdfversion}"><font size="-1">PDF версия<br /><xsl:value-of select="@pdfversion-size"/>Кб</font><br /></a>
									<p><font size="-1">Для просмотра<br />материалов<br />Вам потребуется<br/><a href="http://www.adobe.com/products/acrobat/readermain.html">Acrobat Reader</a></font></p>
								</xsl:if>	
							</td></tr></table>
							</td>
						</tr>
						</table><br/>
					</td>
					
					<td width="20%" colspan="2" class="td-border">
						<div class="main"><ul class="m0"><xsl:apply-templates select="tariff-data[@name='description']"/></ul></div>
					</td>
					
					<td width="50%" colspan="4"  class="td-border">
						<br />
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ПЕРВОНАЧАЛЬНЫЕ ПЛАТЕЖИ</td>
							 <td class="x-small"><br /></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Плата за подключение с оборудованием, приобретенным у МТС<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m1']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v1']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Плата за подключение с собственным оборудованием в офисах продаж МТС / у дилеров&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m2']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v2']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Минимальный первоначальный авансовый платеж&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m3']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v3']"/></font></td>
						 </tr>
						 </table>
						 <br />
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ЕЖЕМЕСЯЧНЫЕ ПЛАТЕЖИ</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Абонентская плата&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m4']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v4']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Обязательная минимальная плата за трафик&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m4p1']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='v4p1']"/></font></td>
						 </tr>
						 </table>
						 <br />
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">SMS</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Исходящие (за одно сообщение)&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m30']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v30_1']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Входяшие (за одно сообщение)&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m31']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v31_1']"/></font></td>
						 </tr>
						 </table>
						 <br/>
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ГОЛОСОВАЯ ПОЧТА</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Прослушивание сообщений в почтовом ящике &nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m29']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v29_1']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Запись сообщения в почтовый ящик&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m29']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1">&nbsp;</font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_1']"/></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v26_1']"/></font></td>
						 </tr>
						 <xsl:if test="not(time-cols-flag='1')">
							 <tr bgcolor="#e1e7f1">
								 <td align="right" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_2']"/></font></td>
								 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v26_2']"/></font></td>
							 </tr>
						 </xsl:if>
						 <xsl:if test="not(time-cols-flag='2')">
							 <tr bgcolor="#cfd7e5">
								 <td align="right" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_3']"/></font></td>
								 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v26_3']"/></font></td>
							 </tr>
						 </xsl:if>
						 </table><br/>
					</td>
				 </tr>
				 <tr bgcolor="#ffffff">
					 <td colspan="8" valign="top" class="td-border">
						 <table border="0" cellspacing="0" cellpadding="0" width="100%"><tr><td bgcolor="#000000" nowrap="true"><spacer type="block" width="1" height="1"/></td></tr></table>
						 <br/>
						 <table border="0" cellpadding="4" cellspacing="1" width="25%" background="">
						 <tr><td bgcolor="#ffb700" align="center" width="50%" class="title">ПЛАТА ЗА ЭФИРНОЕ ВРЕМЯ В МИНУТУ</td></tr>
						 </table>
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr valign="middle">
							 <td><br/></td>
							 <xsl:choose>
								 <xsl:when test="not(time-cols-flag='2')"><td bgcolor="#e1e7f1" colspan="3" align="center"><font size="-1">Время звонка</font></td></xsl:when>
								 <xsl:when test="not(time-cols-flag='1')"><td bgcolor="#e1e7f1" colspan="2" align="center"><font size="-1">Время звонка</font></td></xsl:when>
								 <xsl:otherwise><td bgcolor="#e1e7f1"  align="center"><font size="-1">Время звонка</font></td></xsl:otherwise>
							 </xsl:choose>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td width="25%"><font size="-1"><b>Тарифная зона &#171;Столица&#187;<sup><nobr><font size="-1" color="#ff0000">4</font></nobr></sup> / &#171;Область&#187;  <sup><nobr><font size="-1" color="#ff0000">5</font></nobr></sup></b></font></td>
							 <td width="12%"  align="center" bgcolor="#cfd7e5"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_1']"/></font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="12%"  align="center" bgcolor="#cfd7e5"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_2']"/></font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="12%" align="center" bgcolor="#cfd7e5"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_3']"/></font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#e1e7f1" valign="middle">
							 <td><font size="-1">Исходящий вызов на мобильные телефоны <b>абонентов МТС</b> следующих регионов: Москва и Московская обл., Тульская, Калужская, Псковская, Рязанская, Владимирская, Смоленская, Тверская, Костромская, Ярославская, Нижегородская, Кировская обл., Респ. Коми.&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m21']"/></nobr></font></sup></font></td>
							 <td  width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v21_1']"/> / <xsl:value-of select="tariff-data[@name='v41_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td  width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v21_2']"/> / <xsl:value-of select="tariff-data[@name='v41_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td  width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v21_3']"/> / <xsl:value-of select="tariff-data[@name='v41_3']"/>
							 </font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#cfd7e5" valign="middle">
							 <td><font size="-1">Исходящий вызов на телефоны МГТС, телефоны прочих операторов фиксированной и сотовой связи  г. Москвы и мобильные телефоны <b>абонентов следующих региональных сотовых операторов:</b> РеКом (Орловская, Брянская, Курская, Воронежская, Липецкая, Белгородская области),  ЦСУ 900 (республика Удмуртия)&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m20']"/></nobr></font></sup>
							<p>Входящие звонки, за исключением входящих звонков от абонентов МТС, находящихся в тарифных зонах  Столица  и  Область.</p></font></td>
							 <td width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v20_1']"/> / <xsl:value-of select="tariff-data[@name='v23_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v20_2']"/> / <xsl:value-of select="tariff-data[@name='v23_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v20_3']"/> / <xsl:value-of select="tariff-data[@name='v23_3']"/>
							</font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#e1e7f1" valign="middle">
							 <td><font size="-1">Исходящий вызов на телефоны сети общего пользования Московской области&#160;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m22']"/></nobr></font></sup></font></td>
							 <td width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v22_1']"/> / <xsl:value-of select="tariff-data[@name='v24_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v22_2']"/> / <xsl:value-of select="tariff-data[@name='v24_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v22_3']"/> / <xsl:value-of select="tariff-data[@name='v24_3']"/>
							</font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#cfd7e5" valign="middle">
							 <td><font size="-1">Входящий вызов от&#160;<b>абонентов МТС</b> следующих регионов: Москва и&#160;Московская обл., Тульская, Калужская, Псковская, Рязанская, Владимирская, Смоленская, Тверская, Костромская, Ярославская, Нижегородская, Кировская обл., Респ. Коми., находящихся в&#160;тарифных зонах &#171;Столица&#187; и&#160;&#171;Область&#187;&#160;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m25']"/></nobr></font></sup></font></td>
							 <td  width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v25_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td  width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v25_2']"/>
							</font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td  width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v25_3']"/>
							</font></td></xsl:if>
						 </tr>
						 </table>
					 </td>
				 </tr>
				 <tr bgcolor="#eeeeee">
					 <td colspan="8" align="center" valign="middle" class="td-border">
						 <table cellspacing="8"><tr><td background=""><font size="-1"><b><a href="./tariff_complete.html?id={@id}#comment">Расшифровка сносок, используемых в таблицах</a></b></font></td></tr></table>
						<table cellspacing="8"><tr><td background=""><font size="-1"><b><a href="/coverage/tariff_zone.html">Тарифные зоны и внутрисетевой роуминг...</a></b></font></td></tr></table>
					 </td>
				 </tr>
		

	</xsl:template>

	<xsl:template match="tariffs-long">
			<tr valign="top" bgcolor="ffffff">

				<td width="20%" colspan="2" class="small" rowspan="5">
					<xsl:apply-templates select="phoneNavigationColumn" />
				</td>
					
				<td colspan="4" width="40%" class="td-border">
					<br/>
						<table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ПЕРВОНАЧАЛЬНЫЕ ПЛАТЕЖИ</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Плата за подключение с оборудованием, приобретенным у МТС &nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m1']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v1']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Плата за&#160;подключение с&#160;собственным оборудованием в&#160;офисах продаж МТС / у&#160;дилеров&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m2']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v2']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Минимальный первоначальный авансовый платеж&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m3']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v3']"/></font></td>
						 </tr>
						 </table><br/>
						 
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ПЕРЕАДРЕСОВАННЫЕ ВЫЗОВЫ</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td width="50%" align="right"><font size="-1">Вызовы, переадресованные на междугородный/ международный номер&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m28']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v28_1']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Переадресованные вызовы на телефоны МГТС, др. операторов фиксированной связи г.Москвы, абонентов МТС&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m27']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v27_1']"/></font></td>
						 </tr>
						 </table><br/>
						 
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">SMS</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td width="50%" align="right"><font size="-1">Исходящие (за одно сообщение)&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m30']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v30_1']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Входяшие (за одно сообщение)&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m31']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v31_1']"/></font></td>
						 </tr>
						 </table><br/>
						 
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ГОЛОСОВАЯ ПОЧТА</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Прослушивание сообщений в почтовом ящике &nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m29']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v29_1']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Запись сообщения в почтовый ящик&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m29']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1">&nbsp;</font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_1']"/></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v26_1']"/></font></td>
						 </tr>
						 <xsl:if test="not(time-cols-flag='1')">
							 <tr bgcolor="#e1e7f1">
								 <td align="right" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_2']"/></font></td>
								 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v26_2']"/></font></td>
							 </tr>
						 </xsl:if>
						 <xsl:if test="not(time-cols-flag='2')">
							 <tr bgcolor="#cfd7e5">
								 <td align="right" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_3']"/></font></td>
								 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v26_3']"/></font></td>
							 </tr>
						 </xsl:if>
						 </table><br/>
						</td>
				<td bgcolor="ffffff" colspan="4" width="40%" class="td-border">
						<br/>
						
						<table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">РАЗОВЫЕ ПЛАТЕЖИ</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Добавление/ отмена услуг&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m17']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><a href="http://www.mts.ru/tariffs/sup.html">См. тарифы аб.службы</a><!--h:value-of select="tariff-data[@name='v17']"/--></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Включение голосовой почты&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m16']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><a href="http://www.mts.ru/tariffs/sup.html">См. тарифы аб.службы</a><!--h:value-of select="tariff-data[@name='v16']"/--></font></td>
						 </tr>
						 </table><br/>
						
						<table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						<tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ЕЖЕМЕСЯЧНЫЕ ПЛАТЕЖИ</td>
							 <td align="center" width="50%" class="x-small"><br/></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Абонентская плата&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m4']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v4']"/></font></td>
						 </tr>
						<tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Обязательная минимальная плата за трафик&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m4p1']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:apply-templates select="tariff-data[@name='v4p1']"/></font></td>
						 </tr>
						 <tr  bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Национальный и международный роуминг &nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m6']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v6']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Международный доступ&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m5']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v5']"/></font></td>
						 </tr>
						 <tr  bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Детализированный счет&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m7']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v7']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Доставка счета&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m8']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v8']"/></font></td>
						 </tr>
														<tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Доставка счета по электронной почте&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m9']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v9']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Определитель номера&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m13']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v13']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Антиопределитель номера&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m14']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v14']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Режим ожидания вызова&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m11']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v11']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Конференц-связь/Перевод вызова&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='conference_call_m']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='conference_call']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Запрет вызова&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m12']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v12']"/></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td align="right" width="50%"><font size="-1">Переадресация вызова&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m10']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v10']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Голосовая почта (I/II класс) + переадресация&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m15']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v15']"/></font></td>
						 </tr>
						 <tr bgcolor="#cfd7e5">
							 <td align="right" width="50%"><font size="-1">Добровольная блокировка (каждый месяц)&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m18']"/></nobr></font></sup></font></td>
							 <td align="center" width="50%"><font size="-1"><xsl:value-of select="tariff-data[@name='v18']"/></font></td>
						 </tr>
						 </table><br/>
					</td>
				 </tr>
				 <tr bgcolor="#ffffff" valign="top">
					 <td colspan="8" width="80%" class="td-border">
						 <table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
						 <br/>
						 <table border="0" cellpadding="4" cellspacing="1" width="25%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ПЛАТА ЗА ЭФИРНОЕ ВРЕМЯ В МИНУТУ</td>
						 </tr>
						 </table>
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr valign="middle">
							 <td><br/></td>
							 <xsl:choose>
								 <xsl:when test="not(time-cols-flag='2')"><td bgcolor="#e1e7f1" colspan="3" align="center"><font size="-1">Время звонка</font></td></xsl:when>
								 <xsl:when test="not(time-cols-flag='1')"><td bgcolor="#e1e7f1" colspan="2" align="center"><font size="-1">Время звонка</font></td></xsl:when>
								 <xsl:otherwise><td bgcolor="#e1e7f1"  align="center"><font size="-1">Время звонка</font></td></xsl:otherwise>
							 </xsl:choose>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td width="25%" bgcolor="#ffffff"><br/></td>
							 <td width="12%"  align="center"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_1']"/></font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="12%"  align="center"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_2']"/></font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="12%" align="center"><font size="-1"><xsl:apply-templates select="tariff-data[@name='t19p9_3']"/></font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#ffffff">
							 <td width="25%"><font size="-1"><b>Тарифная зона &#171;Столица&#187;<sup><nobr><font size="-1" color="#ff0000">4</font></nobr></sup></b></font></td>
							 <xsl:choose>
								 <xsl:when test="not(time-cols-flag='1')"><td width="12%" colspan="3" align="center" ><br/></td></xsl:when>
								 <xsl:otherwise><td width="12%" align="center" ><br/></td></xsl:otherwise>
							 </xsl:choose>
							 
						 </tr>
						 <tr bgcolor="#e1e7f1" valign="middle">
							 <td><font size="-1">Исходящий вызов на мобильные телефоны <b>абонентов МТС</b> следующих регионов: Москва и Московская обл., Тульская, Калужская, Псковская, Рязанская, Владимирская, Смоленская, Тверская, Костромская, Ярославская, Нижегородская, Кировская обл., Респ. Коми.&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m21']"/></nobr></font></sup></font></td>
							 <td  width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v21_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td  width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v21_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td  width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v21_3']"/>
							 </font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#cfd7e5" valign="middle">
							 <td><font size="-1">Исходящий вызов на телефоны МГТС, телефоны прочих операторов фиксированной и сотовой связи  г. Москвы и мобильные телефоны <b>абонентов следующих региональных сотовых операторов:</b> РеКом (Орловская, Брянская, Курская, Воронежская, Липецкая, Белгородская области),  ЦСУ 900 (республика Удмуртия).&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m20']"/></nobr></font></sup>
							<p>Входящие звонки, за исключением входящих звонков от абонентов МТС, находящихся в тарифных зонах  Столица  и  Область.</p></font></td>
							 <td width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v20_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v20_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v20_3']"/>
							</font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#e1e7f1" valign="middle">
							 <td><font size="-1">Исходящий вызов на телефоны сети общего пользования Московской области&#160;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m22']"/></nobr></font></sup></font></td>
							 <td width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v22_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v22_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v22_3']"/>
							</font></td></xsl:if>
						 </tr>
						 
						 <tr bgcolor="#ffffff">
							 <td width="12%"  colspan="4"  align="center"><br/></td>
						 </tr>
						 <tr bgcolor="#ffffff">
							 <td width="25%"><font size="-1"><b>Тарифная зона &#171;Область&#187;&#160; <sup><nobr><font size="-1" color="#ff0000">5</font></nobr></sup></b></font></td>
							 <xsl:choose>
								 <xsl:when test="not(time-cols-flag='1')"><td width="12%" colspan="3" align="center" ><br/></td></xsl:when>
								 <xsl:otherwise><td width="12%" align="center" ><br/></td></xsl:otherwise>
							 </xsl:choose>
						 </tr>
						 <tr bgcolor="#e1e7f1" valign="middle">
							 <td><font size="-1">Исходящий вызов на мобильные телефоны <b>абонентов МТС</b> следующих регионов: Москва и  Московская обл., Тульская, Калужская, Псковская, Рязанская, Владимирская, Смоленская, Тверская, Костромская, Ярославская, Нижегородская, Кировская обл.,  Респ. Коми.&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m21']"/></nobr></font></sup></font></td>
							 <td  width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v41_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td  width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v41_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td  width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v41_3']"/>
							 </font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#cfd7e5" valign="middle">
							 <td><font size="-1">Исходящий вызов на телефоны МГТС, телефоны прочих операторов фиксированной и сотовой связи  г. Москвы и мобильные телефоны <b>абонентов следующих региональных сотовых операторов:</b> РеКом (Орловская, Брянская, Курская, Воронежская, Липецкая, Белгородская области),  ЦСУ 900 (республика Удмуртия).&nbsp;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m20']"/></nobr></font></sup>
							<p>Входящие звонки, за исключением входящих звонков от абонентов МТС, находящихся в тарифных зонах  Столица  и  Область.</p></font></td>
							 <td width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v23_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v23_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v23_3']"/>
							</font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#e1e7f1" valign="middle">
							 <td ><font size="-1">Исходящий вызов на телефоны сети общего пользования Московской области&#160;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m22']"/></nobr></font></sup></font></td>
							 <td width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v24_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td width="13%" align="center"><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v24_2']"/>
							 </font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v24_3']"/>
							</font></td></xsl:if>
						 </tr>
						 <tr bgcolor="#ffffff">
							 <xsl:choose>
								 <xsl:when test="not(time-cols-flag='1')"><td width="12%"  colspan="4" align="center" ><br/></td></xsl:when>
								 <xsl:otherwise><td width="12%"  colspan="2" align="center" ><br/></td></xsl:otherwise>
							 </xsl:choose>
						 </tr>
						 <tr bgcolor="#cfd7e5" valign="middle">
							 <td><font size="-1">Входящий вызов от&#160;<b>абонентов МТС</b> следующих регионов: Москва и&#160;Московская обл., Тульская, Калужская, Псковская, Рязанская, Владимирская, Смоленская, Тверская, Костромская, Ярославская, Нижегородская, Кировская  обл., Респ. Коми., находящихся в&#160;тарифных зонах &#171;Столица&#187; и&#160;&#171;Область&#187;&#160;<sup><font color="#ff0000"><nobr><xsl:value-of select="tariff-data[@name='m25']"/></nobr></font></sup></font></td>
							 <td  width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v25_1']"/>
							</font></td>
							 <xsl:if test="not(time-cols-flag='1')"><td  width="13%" align="center"><font size="-1">
								<xsl:value-of select="tariff-data[@name='v25_2']"/>
							</font></td></xsl:if>
							 <xsl:if test="not(time-cols-flag='2')"><td  width="13%" align="center"><font size="-1">
									<xsl:value-of select="tariff-data[@name='v25_3']"/>
							</font></td></xsl:if>
						 </tr>
						 </table><br/>
					 </td>
				</tr>
				<tr bgcolor="#ffffff">
					 <td colspan="8" width="80%" class="td-border">
						 <table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
						 <br/>
						 <table border="0" cellpadding="4" cellspacing="1" width="25%" background="">
						 <tr>
							 <td bgcolor="#ffb700" align="center" width="50%" class="title">ТАРИФЫ НА МЕЖДУГОРОДНЫЕ И МЕЖДУНАРОДНЫЕ РАЗГОВОРЫ</td>
						 </tr>
						 </table>
						 <table border="0" cellpadding="2" cellspacing="1" width="100%" background="">
						 <tr bgcolor="#cfd7e5">
							 <td colspan="9" align="center">
								<font size="-1">
									<xsl:apply-templates select="tariff-data[@name='t31p9_1']"/> <xsl:if test="not(timeint-cols-flag='1')">/ <xsl:apply-templates select="tariff-data[@name='t31p9_2']"/></xsl:if>
								</font>
							 </td>
						 </tr>
						 <tr>
							 <td width="12%"><font size="-1">Зона льготной междугородной тарификации&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m32']"/></font></nobr></sup></font></td>
							 <td width="11%"><font size="-1">Европейская часть России&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m33']"/></font></nobr></sup></font></td>
							 <td width="11%"><font size="-1">Азиатская часть России&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m34']"/></font></nobr></sup></font></td>
							 <td width="11%"><font size="-1">Страны СНГ&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m40']"/></font></nobr></sup></font></td>
							 <td width="11%"><font size="-1">Европейские страны&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m35']"/></font></nobr></sup></font></td>
							 <td width="11%"><font size="-1">Северная Америка&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m36']"/></font></nobr></sup></font></td>
							 <td width="11%"><font size="-1">Африка&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m37']"/></font></nobr></sup></font></td>
							 <td width="11%"><font size="-1">Остальные страны&nbsp;<sup><nobr><font color="#ff0000"><xsl:value-of select="tariff-data[@name='m38']"/></font></nobr></sup></font></td>
						 </tr>
						 <tr bgcolor="#e1e7f1">
							 <td><font size="-1">
								 <xsl:value-of select="tariff-data[@name='v32_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v32_2']"/></xsl:if>
							 </font></td>													
							 <td><font size="-1">													
								 <xsl:value-of select="tariff-data[@name='v33_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v33_2']"/></xsl:if>
							 </font></td>													
							 <td><font size="-1">													
								 <xsl:value-of select="tariff-data[@name='v34_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v34_2']"/></xsl:if>
							 </font></td>													
							 <td><font size="-1">													
								 <xsl:value-of select="tariff-data[@name='v40_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v40_2']"/></xsl:if>
							 </font></td>													
							 <td><font size="-1">													
								 <xsl:value-of select="tariff-data[@name='v35_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v35_2']"/></xsl:if>
							 </font></td>													
							 <td><font size="-1">													
								 <xsl:value-of select="tariff-data[@name='v36_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v36_2']"/></xsl:if>
							 </font></td>													
							 <td><font size="-1">													
								 <xsl:value-of select="tariff-data[@name='v37_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v37_2']"/></xsl:if>
							 </font></td>													
							 <td><font size="-1">													
								 <xsl:value-of select="tariff-data[@name='v38_1']"/><xsl:if test="not(timeint-cols-flag='1')"> / <xsl:value-of select="tariff-data[@name='v38_2']"/></xsl:if>
							 </font></td>													
						 </tr>
						 </table><br/>
						 <table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
					 </td>
				</tr>
				<tr bgcolor="#ffffff">
					 <td colspan="8" width="80%" class="td-border">
						 <a name="comment"/>
						 <table cellspacing="8" background="">
							<tr><td><font size="-1"><xsl:apply-templates select="tariff-comments"/></font></td></tr>
						</table>
					 </td>
				 </tr>
				 <tr bgcolor="#eeeeee">
					 <td colspan="8" align="center" valign="middle" class="td-border">
						 <table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
						<table cellspacing="8" background=""><tr><td><font size="-1"><b><a href="/coverage/tariff_zone.html">Тарифные зоны и внутрисетевой роуминг...</a></b></font></td>
						</tr>
						</table>
					 </td>
				 </tr>

	</xsl:template>

	<!--===================================================-->
	<!--== ТЕЛЕФОНЫ =======================================-->
	<!--===================================================-->

	<xsl:template match="phone">
		<tr bgcolor="#ffffff" valign="top">
		
			<td width="20%" colspan="2" class="small">
				<xsl:if test="count(phone-property)!=0 or count(phone-accessory)!=0"><xsl:attribute name="rowspan">2</xsl:attribute></xsl:if>
				<xsl:if test="count(phone-property)!=0 and count(phone-accessory)!=0"><xsl:attribute name="rowspan">3</xsl:attribute></xsl:if>
				<xsl:apply-templates select="phoneNavigationColumn" />
			</td>
			<td width="20%" colspan="2" class="td-border">
						<table border="0" cellpadding="5" cellspacing="0" width="100%">
						<tr align="center">
							<td background="">
								<br/><a href="http://{@firm-url}" target="_blank"><img src="/i/firms/{@firm-id}.gif" border="0"/></a><br/><br/>
								<table border="0" cellspacing="0" cellpadding="0" width="100%"><tr><td bgcolor="#000000"><img src="/i/d-t.gif" width="1" height="1"/></td></tr></table>
							</td>
						</tr>
						<tr align="center">
							<td><font size="4"><b><xsl:value-of select="@firm"/>&nbsp;<xsl:value-of select="@model"/></b></font></td>
						</tr>
						<xsl:if test = "not(@image='')">
							<tr align="center">
								<td><img src="/i/phones/{@image}" width="{@image_w}" height="{@image_h}"/></td>
							</tr>
						</xsl:if>
						<xsl:if test = "@attr_new or @attr_leader or @attr_mts or @attr_special">
							<tr>
								<td><br/>
									<div class="main">
										<font size="-1">
											<ul class="m0">
												<xsl:if test="@attr_new"><li>Новинка</li></xsl:if>
												<xsl:if test="@attr_leader"><li>Лидер продаж</li></xsl:if>
												<xsl:if test="@attr_mts"><li>Только с SIM картой MTS</li></xsl:if>
												<xsl:if test="@attr_special"><li>Специальное предложение</li></xsl:if>
											</ul>
										</font>
									</div>
								</td>
							</tr>
						</xsl:if>
						<xsl:if test="@docfile">
							<tr>
								<td>
									<div class="main">
										<table border="0" cellpadding="5" cellspacing="0">
										<tr valign="top">
											<td><a href="{@docfile}"><img src="/i/icons/pdf.gif" width="16" height="16" border="0"/></a></td>
											<td><a href="{@docfile}"><font size="-1">Инструкция по эксплуатации (PDF)</font></a></td>
										</tr>
										<tr>
											<td><br /></td>
											<td><font size="-1"><p>Для просмотра материалов Вам потребуется <a href="http://www.adobe.com/products/acrobat/readermain.html"><nobr>Acrobat Reader</nobr></a></p></font></td></tr>
										</table>
									</div>
								</td>
							</tr>
						</xsl:if>
						</table>
					</td>
					<td width="20%" colspan="2" class="td-border">
						<div class="main">
							<xsl:if test="not(@price1='0.00')">
								<p><font size="4" color="red"><xsl:value-of select="@price1"/> у.е.</font><br/>
								(розничная цена при подключении в офисах МТС)</p>
							</xsl:if>
							<xsl:if test="not((@price2='0.00') or (@price2=''))">
								<p><font size="4" color="red"><xsl:value-of select="@price2"/> у.е.</font><br/>
								(розничная цена при подключении в офисах дилеров)</p>
							</xsl:if>
							<xsl:if test="not(@price3='0.00')">
								<p><font size="4" color="red"><xsl:value-of select="@price3"/> у.е.</font><br/>
								(розничная цена без подключения)</p>
							</xsl:if>
						</div>
					</td>
					<td width="20%" colspan="2" class="td-border">
						<div class="main">
							<ul class="m0">
								<xsl:for-each select="phone-description"><li><font color="#000000"><xsl:apply-templates/></font></li></xsl:for-each>
							</ul>
						</div>
					</td>
					<td width="20%" colspan="2" class="td-border">
						<div class="main">
							<xsl:if test="not(@network='')"><b>Стандарт:</b>&nbsp; <xsl:value-of select="@network"/><br/><br/></xsl:if>
							<xsl:if test="not(@size='')"><b>Размеры (В/Ш/Г):</b>&nbsp; <xsl:value-of select="@size"/> мм.<br/><br/></xsl:if>
							<xsl:if test="not(@weight='')"><b>Вес:</b>&nbsp; <xsl:value-of select="@weight"/> гр.<br/><br/></xsl:if>
							<xsl:if test="phone-battery">
								<b>Аккумулятор:</b>
								<ul class="m0"><xsl:for-each select="phone-battery"><li><font color="#000000"><xsl:value-of select="."/></font></li></xsl:for-each></ul>
							</xsl:if>
						</div>
					  </td>
				 </tr>

				 <xsl:if test="count(phone-property)!=0">
				 <tr valign="top">
					 <td width="80%" colspan="8" class="td-border" bgcolor="#ffffff">
						 <table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
						 <br/>
						 <table border="0" cellpadding="4" cellspacing="1" width="25%">
						<tr>
							<td bgcolor="#ffb700" align="center" class="title">ТЕХНИЧЕСКИЕ ХАРАКТЕРИСТИКИ</td>
						</tr>
						</table><br/>
						 <table border="0" cellpadding="4" cellspacing="0" width="100%">
							 <xsl:apply-templates select="phone-property"/>
						 </table>
					 </td>
				 </tr>
				 </xsl:if>

				 <xsl:if test="count(phone-accessory)!=0">
					 <tr valign="top">
						 <td width="80%" colspan="8" class="td-border" bgcolor="#ffffff">
							 <table cellpadding="0" cellspacing="0" border="0" width="100%" height="1" bgcolor="#000000"><tr><td><spacer type="block" width="1" height="1" /></td></tr></table>
							 <br/>
							 <table border="0" cellpadding="4" cellspacing="1" width="25%">
							<tr>
								<td bgcolor="#ffb700" align="center" class="title">АКСЕССУАРЫ</td>
							</tr>
							</table>
							<table border="0" cellpadding="4" cellspacing="0" width="100%">
								<tr bgcolor="#ffffff">
									<td align="right" width="74%" class="td-border"><font size="-1">Наименование акссесуара</font></td>
									<td width="1%" background=""><br /></td>
									<td width="25%" background=""><font size="-1">Цена при покупке с&nbsp;телефоном&nbsp;/&nbsp;без телефона</font></td>
								</tr>
								<xsl:apply-templates select="phone-accessory"/>
							</table>
						 </td>
					 </tr>
				 </xsl:if>
	</xsl:template>

	<xsl:template match="phone-accessory">
		<xsl:if test="not(@price='')">
			<tr bgcolor="#ffffff">
				<xsl:if test = "position() mod 2 = 1"><xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute></xsl:if>
				<td align="right" width="74%" class="td-border"><font size="-1"><xsl:value-of select="."/></font></td>
				<td width="1%" background=""><br /></td>
				<td width="25%" background="">
					<nobr>
						<font size="-1">
							<xsl:if test="string-length(@price!=0)">
								<xsl:value-of select="@price"/>&nbsp;/&nbsp;
								<xsl:choose>
									<xsl:when test="string-length(@price1)!=0">/ <xsl:value-of select="@price1"/></xsl:when>
									<xsl:otherwise><xsl:value-of select="@price"/></xsl:otherwise>
								</xsl:choose>
								у.е.
							</xsl:if>
						</font>
					</nobr>
				</td>
			</tr>
		</xsl:if>
	</xsl:template>

	<xsl:template match="phone-property">
		<tr>
			<xsl:if test="position() mod 2 = 1"><xsl:attribute name="bgcolor">#e1e7f1</xsl:attribute></xsl:if>
			<td align="right" width="74%" class="td-border"><font size="-1"><xsl:apply-templates select="./property"/></font></td>
			<td width="1%" background=""><br /></td>
			<td width="25%" background=""><font size="-1"><xsl:apply-templates select="./value"/></font></td>
		</tr>
	</xsl:template>

	<!--===================================================-->
	<!--== ОТПРАВКА SMS ===================================-->
	<!--===================================================-->
	
	<xsl:template match="form-sendsms">
		<form action="./sent.html" name="send" method="get" onsubmit="return test(this)">
			<input size="" value="1" name="Posted" type="hidden" />
			<p>Мобильный номер должен быть семизначным с префиксом 7095, 7902 или 7910. Например, 7095xxxxxxx.</p>
			<p>Мобильный телефон:<br /><input size="30" value="" name="To" type="text" /></p>
			<p>
				Сообщение: <sup><nobr><font size="-1" color="#ff0000">*</font></nobr></sup><br />
				<textarea rows="5" cols="40" name="Msg" onfocus="ChooseLen()" onchange="ChooseLen()" onkeyup="ChooseLen()" onkeydown="ChooseLen()" onkeypress="ChooseLen()"></textarea><br/>
				<font size="-1">Вы набрали сообщение длиной <input size="4" value="0" name="count" type="text" onfocus="window.document.send.Msg.focus();" /> символов</font>
			</p>
			<p>
				Крайний срок отправки (время суток и дата):<br/>
				<select onChange="" name="SMSHour"><option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option><option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option><option selected="true" value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option></select>
				<select onChange="" name="SMSMinute"><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option><option value="10">10</option><option value="11">11</option><option  selected="true" value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option><option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="28">28</option><option value="29">29</option><option value="30">30</option><option value="31">31</option><option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option><option value="37">37</option><option value="38">38</option><option value="39">39</option><option value="40">40</option><option value="41">41</option><option value="42">42</option><option value="43">43</option><option value="44">44</option><option value="45">45</option><option value="46">46</option><option value="47">47</option><option value="48">48</option><option value="49">49</option><option value="50">50</option><option value="51">51</option><option value="52">52</option><option value="53">53</option><option value="54">54</option><option value="55">55</option><option value="56">56</option><option value="57">57</option><option value="58">58</option><option value="59">59</option></select>
				<br/>
				<select onChange="" name="SMSDay"><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="4">4</option><option value="5">5</option><option value="6">6</option><option value="7">7</option><option value="8">8</option><option value="9">9</option><option value="10">10</option><option value="11">11</option><option value="12">12</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option  selected="true" value="18">18</option><option value="19">19</option><option value="20">20</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="24">24</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="28">28</option><option value="29">29</option><option value="30">30</option><option value="31">31</option></select>
				<select onChange="" name="SMSMonth"><option value="0">января</option><option value="1">февраля</option><option value="2">марта</option><option value="3">апреля</option><option value="4">мая</option><option value="5">июня</option><option value="6">июля</option><option value="7">августа</option><option value="8">сентября</option><option value="9">октября</option><option  selected="true" value="10">ноября</option><option value="11">декабря</option></select>
				<select onChange="" name="SMSYear"><option value="2000">2000</option><option  selected="true" value="2001">2001</option></select>
			</p>
			<input value="Отправить" name="" type="submit" />

			<p><font color="" size="-1"><sup><nobr><font color="#ff0000">*</font></nobr></sup> Посылаемое сообщение должно быть не больше 5 строк (или 160 символов). В противном случае адресат получит два сообщения: одно &mdash; обрезанное до максимально возможного, и второе сообщение - с координатами отправителя.</font></p>
		</form>

		<script language="javascript"><xsl:comment><![CDATA[

			if ((navigator.appName == "Netscape") && (parseInt(navigator.appVersion) == 3))
			{
			mes = "Внимание! Посчитать количество введенных символов Вы сможете только после того, как поле ввода сообщени/я станет не активным - т.е. Вы перейдете к другому полю ввода или просто кликните в пустом пространстве страницы."
			alert(mes);
			}
			var
			codeNumber = new Array(""); codeDigits = new Array(""); function validate(form) {
			var overRun=form.Msg.value.length-160;
			var ok=overRun<=0;
			if (!ok) {alert('Максимальная длина сообщения - 160 символов. Вы превысили лимит на '+overRun+' символов'); return false;}
			else{return true;}
			}
			function ChooseLen() {
			M = window.document.send.Msg.value.length;
			window.document.send.count.value = M;
			}
			function test(form){
			var phone=document.send.To.value;
			var exclude = /(7902|7095|7910)([0-9]{7,7})/
			if (phone.search(exclude) == -1){
			alert('Вы ввели некорректный номер')
			return false;}
			return validate(form)
			}
		]]>
		//</xsl:comment></script>

	</xsl:template>

</xsl:stylesheet>
