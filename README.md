

# external_warzone_cheat
### External warzone cheat with manual mapped driver (function hook), overlay (nvidia hijack), simple esp, no recoil

modern wareafee or warzone or blizzardds whatever their anti cheat is not very good ,,... (excpet their hwid bans are pretty anouning) anywayws thier anti cheat is only usemrode so of course our sexy manual mapped drievr will be veyr nice for this game.... the ovrrlay is pretty fine except for the tranprant flag that gets added [here](https://github.com/NMan1/external_warzone_cheat/blob/7774b0ed2b498ce880edd19a7388f938ac207f6c/modern_warfare/core/overlay/hijack/hijack.cpp#L18) since the nvidia overlay deosnt have the transprent flag we add that but that can be alittle suspicouss for the anit cheat since a new flag has been added to a legit window. The top most is taken care of [here](https://github.com/NMan1/external_warzone_cheat/blob/7774b0ed2b498ce880edd19a7388f938ac207f6c/modern_warfare/core/overlay/renderer/scene.cpp#L40). So if u really care but its really not a massive deal if ur just poarniod u can find a way to rmeove the transprant flag or hook getwindowlong to reutnr nothing who cares its not worth the effort their anticheat is dog shit actually they also maybe take [screenshots too so maybe u should compentsate for that hint hint](https://github.com/NMan1/external_warzone_cheat/blob/7774b0ed2b498ce880edd19a7388f938ac207f6c/modern_warfare/core/overlay/hijack/hijack.cpp#L28) [this](https://www.unknowncheats.me/forum/anti-cheat-bypass/220597-overlay-window-using-ws_ex_topmost-2.html) or [this](https://www.unknowncheats.me/forum/anti-cheat-bypass/349662-method-using-ws_ex_layered-style-overlay.html) can help u regarding the WS_EX_TRANSPARENT  fkag.

esp is very basic but u can expand it if u want there is a draw box function.
no recoil is commented out cuz offset not work but if u want to ad that probaly just create a new thread so esp and recoil are sepreate.

# Vids

https://user-images.githubusercontent.com/44145464/122661983-2b12a680-d155-11eb-844f-a86223c0f5a0.mp4

https://user-images.githubusercontent.com/44145464/122662032-7c229a80-d155-11eb-84cb-53dfba539db9.mp4


# Credits:

https://github.com/es3n1n/nvidia-overlay-renderer/
  - for the hijacking structure
 
albxrn for function hook pasta 

unkowncheats modern warefare fourm for offsets and decprytion
