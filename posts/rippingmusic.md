---
title: Ripping Music
date: 2025-01-23
---


Let's talk about ripping music. It’s important to know what you want before getting started. Here are three common goals:

1. High quality sound.
2. Small file sizes.
3. Exact copies of your CDs or vinyl, with the ability to make your own CDs.

**If you want the best quality:**

Get ready. This can get a bit tricky, and you might need to spend some money. Here’s what I suggest you buy:

- A turntable. 
- A phono amplifier.
- An analogue-to-digital converter that connects via USB.
- Ripping software. If you want something affordable, check out [Alpine Soft](https://www.alpinesoft.co.uk/) for $30. If you want the best, [Pure Vinyl](http://www.channld.com/purevinyl/) is $300. 

Once you have everything set up, it’s pretty straightforward. Just plug in the gear and start ripping your vinyl.

**If small file sizes are your goal:**

You can use a tool called “yt-dlp”. Here’s a command that I use in a alias for years:

```
yt-dlp LINK -x -f 'ba' --audio-format opus --parse-metadata title:%(artist)s - %(title)s.%(ext)s --embed-thumbnail -o %(title)s .%(ext)s
```

In this command, replace `LINK` with the URL of the music you want.

**If you want to sound just like the original CDs or vinyl:**

All you need is a CD burner. They are cheap and easy to find. You also need ripping software. Here are a couple of good options:

- [Paranoia](https://www.xiph.org/paranoia/)
- [EAC](https://www.exactaudiocopy.de/)
- [abcde](https://git.einval.com/git/abcde.git)

I myself use abcde, here's how to set it up:

Install it with your package manager, I myself use Arch BTW so I will be using pacman
  
```
sudo pacman -S abcde
```

Once that's done, you can set it up. Copy the default config to your home directory with this command:

```
cp /etc/abcde.conf ~/.abcde.conf
```

Here is my config:

```
LOWDISK=y


CDDBMETHOD=cddb
CDDBURL="http://gnudb.gnudb.org/~cddb/cddb.cgi"
CDDBCOPYLOCAL="y"
CDDBLOCALDIR="$HOME/.cddb"
CDDBLOCALRECURSIVE="y"
CDDBUSELOCAL="y"

FLACENCODERSYNTAX=flac

# flac binary location
FLAC=flac
FLACOPTS='-s -e -V -8' 
OUTPUTTYPE="flac"
CDROMREADERSYNTAX=cdparanoia            
CDPARANOIA=cdparanoia  
CDPARANOIAOPTS="--never-skip=40"

CDDISCID=cd-discid            
                               
OUTPUTDIR="~/mus"               

ACTIONS=cddb,playlist,read,encode,tag,move,clean
              
OUTPUTFORMAT='${OUTPUT}/${ARTISTFILE}-${ALBUMFILE}/${TRACKNUM}.${TRACKFILE}'
VAOUTPUTFORMAT='${OUTPUT}/Various-${ALBUMFILE}/${TRACKNUM}.${ARTISTFILE}-${TRACKFILE}'
ONETRACKOUTPUTFORMAT='${OUTPUT}/${ARTISTFILE}-${ALBUMFILE}/${ALBUMFILE}'
VAONETRACKOUTPUTFORMAT='${OUTPUT}/Various-${ALBUMFILE}/${ALBUMFILE}'
PLAYLISTFORMAT='${OUTPUT}/${ARTISTFILE}-${ALBUMFILE}/${ALBUMFILE}.m3u'
VAPLAYLISTFORMAT='${OUTPUT}/Various-${ALBUMFILE}/${ALBUMFILE}.m3u'

mungefilename ()
{
  echo "$@" | sed -e 's/^\.*//' | tr -d ":><|*/\"'?[:cntrl:]"
}

MAXPROCS=2                              
PADTRACKS=y                             
EXTRAVERBOSE=2                          
EJECTCD=y                               
```


After all this, just run the command `abcde` and it will start ripping. It saves the files in your Music folder (In my conifg I use ~/mus, you can see in my dotfiles) and even ejects the disc when it’s done. Pretty cool, right? 

Now you know how to rip music based on what you need! Good luck.