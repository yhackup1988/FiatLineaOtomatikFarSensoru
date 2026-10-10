# FiatLineaOtomatikFarSensoru
## Fiat Linea Araçlar için Otomatik Far Sensörü Projesi 2. Versiyon
Tünele girerken far açmayı unutmak, akşam üstü gölgeli yolda farsız ilerlemek, sabah güneş doğduktan sonra farları kapatmayı akıl edememek… Hepimizin başına geldi. Üstelik her aracın otomatik far sistemi yok, olanların da ayarı genelde sabit.


![Splash](https://blog.yakupulutas.com/wp-content/uploads/2026/10/image-1024x658.png)


Bu yüzden kendi otomatik far sistemimi yapmıştım yıllar önce o yazıyı buradan okuyabilirsiniz şimdi projeyi bir üst seviyeye taşıdım.
Bir Arduino Nano, bir LDR (ışığa duyarlı direnç) ve küçük bir OLED ekran. Sistem ortam ışığını ölçüyor, park lambasını, kısa farı ve gündüz farını (DRL) kendisi yönetiyor. Eşiklerin, gecikmelerin ve parlaklıkların hepsi cihazın kendi menüsünden ayarlanabiliyor.


Bu yazıda sistemin nasıl çalıştığını, ekranda gördüklerini ve menüdeki her ayarın ne işe yaradığını anlatıyorum.

![Splash](https://blog.yakupulutas.com/wp-content/uploads/2026/10/image-6-768x485.png)
## Proje ne yapıyor?
Ortam karardıkça önce park lambasını, daha da kararınca kısa farı yakar.
Işık geri gelince farları, kısa süreli gölgelerde yanıp sönmesinler diye bir bekleme süresinin ardından söndürür.
Gündüz farını (DRL) sabit bırakmaz. Ortam ışığına göre parlaklığını yumuşakça değiştirir.
Tünel gibi ani karanlıklarda beklemeden devreye girer.
Marş sırasında farları kapalı tutan bir mars gecikmesi vardır.
Tüm ayarlar ekrandaki menüden yapılır ve cihaza kaydedilir.


[![Otomatik Far Sensörü](https://img.youtube.com/vi/GgLcV_yzKG8/0.jpg)](https://youtube.com/shorts/GgLcV_yzKG8)

## Sistem nasıl karar veriyor?
### LDR ve ışık değeri
LDR ortamın ne kadar aydınlık olduğunu ölçer ve değer 0 ile 1023 arasında okunur. Bu yazıda karanlıkta değerin düştüğü bir bağlantı varsayılmıştır. Değer düştükçe sistem önce park lambasını, sonra kısa farı açar.

Ölçümler tek seferlik değildir. Sistem 50 milisaniyede bir kısa aralıklarla birkaç örnek alıp ortalamasını hesaplar, ardından bu değeri bir önceki değerle harmanlar. Böylece lamba titreşimi ya da anlık parlamalar farları şaşırtmaz. Bu harmanlamanın hızını menüden ayarlayabilirsin (Sensör Hızı).


### Gecikme: boşuna yanıp sönmeyi engeller
Işık eşiğin altına inse bile, aşağıda kalma süresi dolmadan far açılmaz. Aynı şekilde, ışık geri geldiğinde de far hemen sönmez. Bir ağacın gölgesi ya da kısa bir köprü geçişi farı yakıp söndürmesin diye bu bekleme süreleri ayrı ayrı ayarlanabilir.

### Histerezis: eşiğin etrafında titremeyi engeller
Farın açılma ve kapanma eşiği birbirinden farklıdır. Açılma eşiği ayarladığın değerdir, kapanma eşiği ise bu değerin biraz üstüdür. İkisi arasındaki fark histerezis ayarıdır.

Örnek (varsayılan değerlerle, park lambası): Işık değeri 400’ün altında 2 saniye kalırsa park lambası yanar. Değer 440’ın üstünde (400 + 40 histerezis) 6 saniye kalırsa söner. 400 ile 440 arasında hiçbir şey olmaz ve sayaçlar sıfırlanır. Böylece değer eşik civarında gidip gelse bile far titremez.

### Tünel modu
Işık yarım saniye içinde 150 birimden fazla düşerse ve değer Kısa Eşik’in altına inerse (örneğin bir tünele girişte), sistem açma gecikmesinin beklenmesini atlayıp farları hemen yakar.

### Mars gecikmesi
Kontak açıldıktan sonra belirli bir süre boyunca farlar kapalı tutulur. Marş basarken şarj dinamosu yüksek amper çektiği için ekstra farların sisteme yük bindirmesini engelliyoruz. Süre, cihaza enerji verildiği andan itibaren sayılır ve açılış animasyonu da bu sürenin içindedir.

### DRL: ışığa göre yumuşak parlaklık
Gündüz farı sabit parlaklıkta değildir:

Işık, DRL Tam Eşik değerinin üstündeyse DRL en parlak halindedir (DRL Gündüz PWM).
Işık, Kısa Eşik ile Tam Eşik arasındaysa DRL ışığa göre kademeli olarak kısılır.
Kısa far yanıyorsa DRL sabit olarak DRL Kısa PWM parlaklığına iner.
Parlaklık ani değişmez, saniyede yaklaşık 100 adım hızıyla yumuşak geçiş yapar.

## Ekranda neler görüyorsun?
### Ana ekran
| Bölge | Ne gösterir? |
| --- | --- |
| Sol üst rozet | AUTO: sistem çalışıyor. KAPALI (yanıp söner): sistem geçici olarak kapatıldı. |
| Rozetin yanı | Kullanılan sensör pini ve anlık ışık değeri (örn. A0:512). |
| PARK kartı | Park lambası. Yanıyorsa kart beyaza dolar. |
| KISA kartı | Kısa far. Yanıyorsa kart beyaza dolar. |
| DRL kartı | Gündüz farı. Yüzde değeri ve parlaklık çubuğu gösterir. |
| Alt çubuk | Anlık ışık seviyesi. Çubuğun üstündeki iki küçük çizgi eşikleri gösterir: soldaki Kısa Eşik, sağdaki Park Eşik. |
Bir far açılmak ya da kapanmak üzereyken kartın içinde geri sayım ve bir ilerleme çubuğu görünür. AC 3 ifadesi “3 saniye sonra açılacak”, KAP 5 ifadesi “5 saniye sonra kapanacak” demektir. Böylece sistemin neden henüz açmadığını bir bakışta görebilirsin.

### Tuşlar ve geçici kapatma
| Tuş | Menüde | Ayar düzenlerken |
| --- | --- | --- |
| Yukarı / Aşağı | Listede gezinir | Değeri artırır / azaltır |
| OK | Seçili öğeyi açar | Kaydeder ve listeye döner |
| Geri | Ana ekrana döner | Kaydeder ve listeye döner |
Ana ekranda OK menüyü açar.

## Menüdeki ayarlar
Aşağıdaki tabloda her ayarın varsayılan değeri, aralığı ve adım büyüklüğü var:

| # | Ayar | Varsayılan | Aralık / Adım |
| --- | --- | --- | --- |
| 1 | Sistem Durumu | AKTİF | AKTİF / KAPALI |
| 2 | Sensör Pin | A0 | A0, A1, A2, A3, A6, A7 |
| 3 | Park Eşik | 400 | 10–1000, adım 10 |
| 4 | Kısa Eşik | 170 | 10–1000, adım 10 |
| 5 | Histerezis | 40 | 0–200, adım 5 |
| 6 | Park Aç Geç | 2,00 sn | 0–30 sn, adım 0,25 sn |
| 7 | Park Kap Geç | 6,00 sn | 0–30 sn, adım 0,25 sn |
| 8 | Kısa Aç Geç | 1,50 sn | 0–30 sn, adım 0,25 sn |
| 9 | Kısa Kap Geç | 4,00 sn | 0–30 sn, adım 0,25 sn |
| 10 | DRL Gündüz PWM | %100 | %0–100, adım 5 |
| 11 | DRL Tam Eşik | 700 | 10–1000, adım 10 |
| 12 | DRL Kısa PWM | %58 | %0–100, adım 5 |
| 13 | Ekran Koruyucu | 1 dk | Yok / 1 dk / 5 dk / 30 dk |
| 14 | Sensör Hızı | Orta | Çok Yavaş … Çok Hızlı (5 kademe) |
| 15 | Mars Gecikme | 5 sn | 1–15 sn, adım 1 sn |
| 16 | Fabrika Ayarı |  | Onaylı sıfırlama |
| 17 | Hakkında |  | Bilgi ekranı |
| 18 | Test Modu |  | Canlı ışık değeri |
### Sistem Durumu
Sistemi geçici olarak açıp kapatır. Ana ekrandaki uzun basışla aynı işi yapar. Kaydedilmez, her açılışta AKTİF başlar.

### Sensör Pin
LDR’nin bağlı olduğu analog pini seçer. Pin değiştirildiğinde sensör yeniden başlatılır.

### Park Eşik
Işık değeri bu seviyenin altına düşüp Park Aç Geç süresi boyunca altında kalırsa park lambası yanar. Değeri yükseltirsen park lambası daha erken, yani alacakaranlıkta daha aydınlıkken yanar.

### Kısa Eşik
Kısa farın açılma seviyesidir. Park Eşik’ten daha düşük olmalıdır. Böylece önce park lambası, ortam daha da kararınca kısa far yanar.

### Histerezis
Farın kapanma seviyesi, açılma eşiğinin bu değer kadar üstüdür. Park için kapanma seviyesi Park Eşik + Histerezis, kısa için Kısa Eşik + Histerezis olur. Değeri küçültürsen far, eşik civarında daha kolay yanıp sönebilir. Büyütürsen far, ışık eşiği biraz geçtikten sonra bile açık kalır.

### Park Aç Geç / Kısa Aç Geç
Işık eşiğin altına indikten sonra farın yanması için beklenen süredir. Kısa gölgelerde farın boşuna yanmasını engeller. Çok küçük yaparsan sistem tepkili olur ama yanlış tetiklenebilir.

### Park Kap Geç / Kısa Kap Geç
Işık eşiğin üstüne çıktıktan sonra farın sönmesi için beklenen süredir. Tünelden çıkışta ya da kısa bir aydınlık geçişte farların hemen sönmesini engeller. Bu süreleri açma sürelerinden daha uzun tutmak güvenlik açısından mantıklıdır.

### DRL Gündüz PWM
Gündüz farının en parlak halidir. LED sürücüye giden PWM sinyalinin yüzdesidir.

### DRL Tam Eşik
DRL’nin Gündüz PWM parlaklığına ulaştığı ışık seviyesidir. Işık bu değerin üstündeyse DRL tam parlaktır. Işık, Kısa Eşik ile Tam Eşik arasındaysa DRL ışığa göre kademeli olarak kısılır. Ortam karardıkça DRL’nin yumuşakça sönmesini sağlayan ayar budur. Tam Eşik değeri, Kısa Eşik’ten en az 10 birim yüksek tutulur. Daha düşük girilirse sistem bunu otomatik olarak düzeltir.

### DRL Kısa PWM
Kısa far yanarken DRL’nin ineceği parlaklıktır. Kısa far açıkken DRL’nin sabit ve düşük bir değerde kalması istenir. Bu ayar o değeri belirler.

### Ekran Koruyucu
Belirlenen süre boyunca hiçbir tuşa basılmazsa ekran hareketli bir animasyona geçer: kayan yıldızlar ve ekranda süzülen bir far ikonu. Animasyon en düşük parlaklıkta çalışır. 10 dakika sonra ekran tamamen kapanır. Herhangi bir tuşa basıldığında ana ekran döner. Bu ilk basış ayrıca bir komut çalıştırmaz.

Sistem KAPALI durumdayken ekran koruyucu devreye girmez, uyarı ekranda kalır. Yok seçilirse ekran hiç değişmez.

### Sensör Hızı
Işık ölçümünün ne kadar tepkili olacağını belirler:

| Kademe | Davranış |
| --- | --- |
| Çok Yavaş | Çok kararlı, ani değişimlere yaklaşık 1 saniyede tepki verir. |
| Yavaş | Kararlı. |
| Orta | Dengeli (varsayılan). |
| Hızlı | Tepkili. |
| Çok Hızlı | Ham ölçüme en yakın, gürültüye en açık. |
Işık değeri ekranda sürekli zıplıyorsa kademeyi yavaşlat. Tepkiyi çok yavaş buluyorsan hızlandır.

### Mars Gecikme
Enerji verildikten sonra farların kapalı kalacağı süredir. Marş sırasındaki voltaj dalgalanmasında farların yanıp sönmesini engeller.

### Fabrika Ayarı
Tüm ayarları varsayılan değerlere döndürür. Yanlışlıkla basılmasın diye onay ister: OK ile onaylanır, Geri ile vazgeçilir. Sistem tekrar AKTİF duruma gelir.

### Hakkında ve Test Modu
Hakkında sürüm ve geliştirici bilgisini gösterir. Test Modu anlık ışık değerini büyük rakamlarla ve bir çubukla gösterir, eşikleri ayarlarken en önemli aracındır.

## Eşikleri nasıl ayarlarsın?
Test Modu‘nu aç.
Aracı farklı ışık koşullarında dene: parlak gündüz, gölge, alacakaranlık, tam gece.
Her koşulda ekranda görünen değeri not et.
Park Eşik‘i alacakaranlıkta, park lambasının yanmasını istediğin değerin biraz üstüne ayarla.
Kısa Eşik‘i daha karanlık bir değere, kısa farın yanmasını istediğin yere ayarla.
DRL Tam Eşik‘i, DRL’nin tam parlak olmasını istediğin aydınlık değerine ayarla.
Yola çıkmadan önce Aç Geç / Kap Geç sürelerini gerçek sürüşte dene. Gölgeli yollarda farlar sık yanıp sönüyorsa kapanma gecikmelerini ve histerezisi artır.
LDR’nin aracın içinde nereye takıldığı değerleri ciddi biçimde etkiler. Sensörü yerinden oynattığında eşikleri yeniden kontrol et.

## OLED ekranı korumak
OLED ekranlarda aynı görüntünün uzun süre sabit kalması kalıcı iz bırakabilir. Bu yüzden sistemde bir ekran koruyucu var ve ekran koruyucuda hiçbir piksel sabit kalmaz. Yine de en güvenli seçenek ekranın tamamen kapalı olmasıdır. Animasyon süresini ve koruyucunun devreye girme zamanını kendi kullanımına göre ayarlayabilirsin.

## Son söz
Otomatik Far Sensörü v2, ucuz parçalarla yapılmış ama günlük kullanımda gerçekten işe yarayan bir sistem. En çok önem verdiğim noktalar şunlar oldu: farların boşuna yanıp sönmemesi, kapalı unutulup karanlıkta kalınmaması ve ayarların cihazın başında, bilgisayara ihtiyaç duymadan yapılabilmesi.

Sorularını ya da önerilerini yorumlarda bekliyorum.

## Kullandığım parçalar
- Arduino Nano Geliştirme Kartı ve Dişi Header Grubu
- LM7808 veya LM7809
- 128×64 I2C OLED ekran (SSD1306) (4 Butonlu)
- LDR
- BC548 veya BC547 transistör (veya benzer bir Npn)
- 220R ile 330R arası bir Direnç, 1K ve 10k Direnç
- 104 Mercimek Kondansatör
- 200mA Cam sigorta ve Yuvası
- 1N4007 Diyot
- 6×13 Delikli Bakır Plaket
- 33uF 16V ve 100uF 25V Kondansatör
- Isı ile Daralan Makaron 5mm
### İsteğe Bağlı Malzemeler

- Yeteri miktarda Klemens (Kabloyu direk karta da lehimleyebilirsiniz)
- TIP121 DRL (Gündüz Ledleri) için (Gündüz lediniz yoksa gerek yok)
- Plastik Kutu Siyah (58x92x27mm) (Ben yıllarca kartsız kullandım hiç bir şey olmadı. (https://blog.yakupulutas.com/otomatik-far-sensoru-nasil-yapilir/)
- 1×40 180C Dişi Header (Arduinoyu direk karta da lehimleyebilirsiniz)
## Devre Şeması;
Karışık görünüyor olabilir. aklınıza takılan soruları yorumlardan sorabilirsiniz.


## Arduino’ya yüklenmesi gerek kod;
Koda github’dan erişebilirsiniz Public’tir. Doğrudan Arduino’nuza yükleyebilirsiniz.

https://github.com/yhackup1988/FiatLineaOtomatikFarSensoru
