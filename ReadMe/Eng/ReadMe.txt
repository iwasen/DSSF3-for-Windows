================================
DSSF3 Version 5
--------------------------------
Ver.5.2.0.x
by Yoshimasa Electronic Inc.
================================

Thank you very much for downloading DSSF3.
DSSF3 is the advanced sound measurement/analysis software and is frequently updated.
Users of DSSF3 version 5 can online update to version 5.2.0.x freely.
This is the latest version that includes all updates until Aug 2025.


**************************************************
About version 5.2.0.x
**************************************************

See the DSSF3 news since Nov 2005 for more about the updated functions.

http://www.ymec.com/news/dssf3e.htm


**************************************************
About trial period and limitation of functions
**************************************************
---Trial periodÅi30 days)---
All functions are available

---Limitation after trial period---
/RA/
Signal generator: No limitation
FFT analyzer: Available only for five seconds
Oscilloscope: Available only for five seconds
THD analyzer: Available only for five seconds
Impulse response: Load/Save file, output wave file are NOT available
Running ACF: Load/Save file are NOT available
Recorder: No limitation
Preset: No limitation
/SA/
Calculation of acoustical parameters is NOT available
/EA/
Save data and load wave file are NOT available

---After registration for RA Light ---
Signal  generator, FFT analyzer, Oscilloscope, Recorder, Preset in RA are available

---After registration for RA---
All functions of RA are available

---After registration of SA---
All functions of SA are available

---After registration of EA---
All functions of EA are available

<< Note >>
DSSF3 works on Windows XP, Vista, 7, 8, 10.

Before purchasing, install the program and confirm the proper operation. DSSF3 has four versions, but has common installer file. After the installation, all functions can be used during a trial period of 30 days.

After the trial period, almost all functions will be restricted by the password protection. Purchase the license of registration to release the protection.

It allows three installs and activations.

**************************************************
Installation
**************************************************
Run the installer and follow the instruction below.
1. InstallShield wizard will start up. Click the "Next" button.
2. The Product License Agreement will appear. Use the "scroll arrow" button to read through the text, and if you agree to its terms, click the "I accept the terms in the license agreement"; this will activate the "Next" button and allow you to proceed. If you do not agree to the terms, please discontinue the installation.
3. Enter (or confirm) "User Name" and "Organization", and click on "Next".
4. The default installation folder is "C:/Program Files/DSSF5E". (If you wish to install the program in a different folder, press the "Change" button on the right, and specify the folder for installation.) Click on "Next".
5. Select a setup type. Usually select "Default" and click the "Next".
6. A screen will appear showing the settings for installation. Check that these settings are correct, and press the "Install" button. This will begin the installation.
7. When the message "InstallShield Wizard completed" appears, the installation is complete. Press the "Finish" button to close the dialog box.

<< Note >>
After the installation, do not forget to execute the online update when you start the program. This operation maintains a program at the latest version at that time. We recommend you to visit the following URL sometimes to see the update information.
http://www.ymec.com/products/dssf3e/index.htm



**************************************************
List of main functions
**************************************************
The components of DSSF3 are: the Realtime Analyzer (RA), the Sound Analyzer (SA), and the Environmental noise Analyzer (EA).

RA [2 channel real-time analyzer]
1. Peak level meter
2. Signal generator
3. Power spectrum
4. Octave analysis (1/1 1/3 1/6 1/12 and 1/24)
5. Sound level meter (Calibration is needed)
6. TEF (time / energy / frequency) analysis
7. Oscilloscope
8. THD (Total Harmonic Distortion) analyzer
9. Phase meter
10. Real time display of the correlation functions (auto- and cross-)
11. Spectorogram analyzer
12. Impulse response measurement
13. Running ACF measurement
14. Recorder Player

EA [2 channels environmental noise analyzer]
1. Automatic measurement by the time specification
2. Automatic measurement by the sound level specification
3. Noise source identification by the acoustical parameters

SA [2 channels sound analyzer]
1. Impulse response analysis
2. Running ACF analysis
3. Environmental noise analysis


**************************************************
Application fields
**************************************************
Measurement in a concert hall
Measurement of home audio and car audio system
Tuning of musucal instruments and sound analysis
Environmental noise measurement (aircraft noise, traffic noise, living noise)
Sound quality evaluation of industrial products
Biomedical sound measurement
etc.

We are appreciated if you could tell us your purpose of use for DSSF3. If you have any comment or request to the software, please tell us. We will make use of your feedback to improve the products.
Email: shop@ymec.com


**************************************************
Operating Environment
**************************************************
OS: Windows 7/8/10/11
Sound Functions: Some functions are completely applicable to 2-channel systems
See the technical support page for more information.
http://www.ymec.com/store/en/manuals.htm


**************************************************
Additional equipment
**************************************************
See the technical support page when you consider to purchase the other equipments, such as microphone and amplifier.
http://www.ymec.com/store/en/manuals.htm


**************************************************
Operation Check
**************************************************

http://www.ymec.com/manual/install/eg.htm

<< Realtime Analyzer >>
>From the Start menu choose Programs -> Acoustic Analyzing System 5E -> Realtime Analyzer, and start the program. Press the "Trial" button in the dialog box. This program can be used (= You can press the "Trial" button) for 30 days on a trial basis.

1. Set the Input Device as "WAVE/DirectSound" from the main window of Realtime Analyzer. (Device name may vary depending on the model of PC.) This setting allows input of internal audio signals rather than from an external microphone.


2. Open the Windows volume control. Check the mute for other sources than WAVE. This setting allows output of the Signal Generator.


3. Click the "Signal Generator" button in the main window. Set the Frequency at 1000Hz and Waveform as Sinusoidal, and increase the Output Level from 0 with the Interlock box checked; now press the "Start" button. You should hear a high "beep" sound from your PC's speaker.


4. Check the "Input" box of the "Peak Level Monitor" on the main window, then click the "Oscilloscope" button. Now click the "Start" button at the bottom right of the oscilloscope. The sinusoidal wave should be displayed on the screen. If the top of the waveform is distorted, this indicates an excessive input. Reduce the volume of the "Input Device" on the main window until the "Input" signal in the "Peak Level Monitor" is smaller than 0db.

An input and an output sound device works properly if the operations so far is confirmed.


<<Troubleshooting >>
Troubles on the operation of RA are mainly caused by the version of Windows, installation problem, and the setting of the soundboard. Try the follwing troubleshooting tips first.


1. The case when the registration number is required every time or the program does not start
In many cases, the program is not installed properly. This happens in Windows before the 2nd edition of Windows 98. The problem in this case is that Windows update has not been carried out.  If so, the same problem should always occur when installing the new software. Try the Windows update first, and reinstall the program. Delete the program before reinstall it. When software does not work on old PC and works with new PC, please use it with new PC.


2. Setup of soundboard
DSSF3 works with a SoundBlaster compatible soundboard. Soundcard of almost all famous PC (DELL, COMPAQ, SONY, TOSHIBA, NEC, FUJITSU, IBM, and so) can be used. If you use the sound interface of Windows, it works as it is. Basically, the soundcard for almost all PC has satisfied the Windows interface. So when not working, failure of a soundcard and a PC can be considered.

Sometimes a problem may not become clear unless you make DSSF3 operate on another PC. When a special soundcard is used, it may not work with DSSF3 if the soundcard does not follow the Windows interface. In this case, DSSF3 may work well when you stop the soundcard and use the default soundcard.

If these tips cannot help, please let us know. We will support you by email or telephone.


**************************************************
Payment
**************************************************
Select your payment method from PAYPAL, credit card or bank transfer.
If you buy a lot of licenses, refer to the "Other purchase" below.

---- Price list ----
DSSF3          98USD
DSSF3 Light    39USD


---- PAYPAL ----

Pay me securely with any major credit card through PayPal !

http://www.ymec.com/store/en/

Please transfer to ymecsaku3@ymec.com

---- Credit card ----
You can pay by credit card (VISA/MASTER/AMEX/DINERS/JCB) via our web site. (SSL, secure site)

http://www.ymec.com/store/en/

As soon as payment is completed, our order confirmation email which contains the details about delivery will be sent to you.


---- Bank transfer ----

If you have any requests, please inquire by email.


---- Other purchase ----

If you need an invoice, a receipt and so on, we will issue them. Furthermore, if you ask for other settlement procedure for trade, please let us know. As for various requests, please contact us.


------------------------------------------------
Masatsugu Sakurai
shop@ymec.com
Yoshimasa Electronic Inc.
Dai-ichi Nishiwaki Bldg. 1-58-10 Yoyogi, Shibuya-ku,
Tokyo 151-0053 JAPAN
Phone: +81-3-3370-5160
FAX: +81-3-3370-6548
------------------------------------------------


**************************************************
User Registration
**************************************************

http://www.ymec.com/store/en/register.htm

1. Run "Acoustic Analyzing System 5E", and press "Registration" button in the dialog box that appears immediately after startup.

2. A "Hard Key" (an 8-digit number) will be displayed in the dialog box that appears next; send this number by e-mail. We will send your "Registration Number" by return e-mail.

3. Enter the "Registration Number" in the above-mentioned "Registration" dialog box, and press the "OK" button.

You can now use this software indefinitely.

Please note, however, that your user registration will become invalid if you change your PC or reinstall your OS. If you send a request, we will re-issue your Registration Number (the number of reissue is determined in accordance with the products).


**************************************************
Program manuals, sound measurement guide, FAQ
**************************************************
There are helpful documents in the following URLs.
When you go out to measurement, print out them and take it with you if needed.

Program manual
(RA) http://www.ymec.com/manual/era/
(SA) http://www.ymec.com/manual/esa/
(EA) http://www.ymec.com/manual/eea/

Easy Introduction to Sound Measurement using a Notebook PC
http://www.ymec.com/hp/signal2/

Questions and Answers for DSSF3
http://www.ymec.com/products/qa2/

<< Note >>
We guess your PC is probably not online in most cases when you conduct measurement. Such a situation does not enable you to refer to above online manual. So we recommend that you download above documents to your PC beforehand before you go out to measurement.


**************************************************
Image database software MMLIB
**************************************************
Image files of the measurement screen are easily managed by our electronic filing software MMLIB.

MMLIB web page

http://www.ymec.com/products/mmlibe/

MMLIB Download

http://www.ymec.com/download/emmlib.htm


**************************************************
Online Updates
**************************************************
We will be updating this program according to need; you can receive upgraded versions via the Internet.
Please execute "Online Update" from the "Help" menu.


**************************************************
Others
**************************************************
Please direct questions or comments to:
E-Mail: shop@ymec.com
URL: http://www.ymec.com/


**************************************************
Conditions of Use
**************************************************
Yoshimasa Electronic Inc. (hereinafter referred to as "Yoshimasa") grants to the customer the right to use the software program (hereinafter referred to as "the Program") provided with these Conditions of Use, based on the Articles outlined below, provided that the customer agrees to said Articles.
The customer shall take full responsibility for the selection of the Program so as to obtain the expected results, as well as for installation of the Program, applications, and application results.

1. Granting of Usage Rights
Yoshimasa grants to the user the right to use the Program according to the conditions outlined here.

2. Term of Use
(1) These usage conditions shall be effective from the time the Program is installed by the customer.
(2) In the event that the customer violates any of these usage conditions, Yoshimasa may revoke usage rights for the Program at any time. In this event, the customer must destroy all copies and all components of the Program.

3. Usage Rights
(1) The customer may use the Program on only one computer.
(2) The Program is considered to be "used" on a given computer when it loaded onto the temporary memory (e.g., RAM) or fixed memory (e.g., hard disk or other memory device) of the computer in question.

4. Copyrights
The copyrights and all other rights related to the Program (including software, manual, accompanying documents, etc.) shall remain the property of Yoshimasa.

5. Copying, alteration, or integration of the Program.
(1) The customer may not copy, alter, integrate, or undertake any other processing of the Program.
(2) The customer may not, under any circumstances, copy manuals or other related documents provided with the Program.
(3) These usage conditions do not constitute a transfer to the customer of intangible property rights related to the Program.

6. Prohibition of Transfer of the Program, etc.
The customer may not copy, sell, distribute, hand over, or loan the Program or any part of the Program to a third party, or otherwise allow a third party to use the Program.

7. Prohibition of Reverse Compiling, etc.
The customer may not subject the Program to reverse engineering, reverse compiling, or reverse assembly.

8. Guarantee Limitations
Yoshimasa offers no guarantees whatsoever with regard to the Program. The customer shall take full responsibility for any problems arising in relation to the Program, and shall bear all costs resulting from such problems.

9. Exemption from Responsibility
Yoshimasa shall bear no responsibility whatsoever in the event that losses are suffered either directly or indirectly in relation to use of the Program.