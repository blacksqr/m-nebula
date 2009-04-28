#!/usr/bin/python

import traceback, npython, site, sys, time
sys.path.append('c:\\development\\python\\dist\\src\\lib\\lib-tk')
sys.argv=[""]    # needed to allow Tkinter to load in a module..
from Tkinter import *

import nsh, observerlib

class SourceView:
    text = None
    
    def __init__(self, demo, scripts):
        self.demo, self.scripts = demo, scripts

    def setSource(self, source):
        if not self.text or not self.text.winfo_exists():
            t = Toplevel()
            f = Frame(t)
            self.text = Text(f)
            scroll = Scrollbar(f, command=self.text.yview)
            self.text['yscrollcommand'] = scroll.set
            self.text.grid(row=0, column=0)
            scroll.grid(row=0, column=1, sticky=N + S)
            f.pack(expand=1, fill=BOTH)
        self.text.delete(0.0, END)
        self.text.insert(END, source)

    def viewSource(self):
        index = self.demo.get()
        source = open(self.scripts[index].file, 'r').read()
        self.setSource(source)


class Demo:
    def __init__(self, name, file, brightness, *args):
        self.name, self.file, self.brightness, self.args = name, file, brightness, args

class TekDemo:
    scripts = [
        Demo("Mesh Interplation", 'meshanim.py', 0.5),
        Demo("Mesh Mixing", 'meshmix.py', 0.5),
        Demo("Skin Deformation", 'boneanim.py', 0.5),
        Demo("Hierarchical Animation", 'newplanets.py', 0),
        Demo("Animated Light", 'lightanim.py', 0.2),
        Demo("Flip Flop (Texture)", 'flipflop.py', 0.5),
        Demo("Shaders", 'shadernode.py', 0),
        Demo("Flat Terrain", 'flatTerrain.py', 0, [15,10,0], [0,0,0], [30]),
        Demo("Bigger Flat Terrain", 'flatterrain1.py', 0, [20,10,0], [-20,0,0], [40]),
        Demo("Billboards + Sprites", 'billboards.py', 0, [.7,0,0], [-5,25,0], [22]),
        Demo("Spherical Terrain", 'sphericalTerrain.py', 0, [0,5,0], [0,0,0], [20]),
        Demo("Static Mesh Emitter", 'staticmeshemitter.py', 0),
        Demo("Mesh Particle Emitter", 'meshemitter.py', 0, [0,0,0], [0,0,0], [5]),
        Demo("Shadows", 'shadow.py', 0, [], [], [40]),
        Demo("Alpha Test", 'alphatest.py', 0),
        Demo("Exponential Fog", 'fog.py', 0, [0,5,0], [0,0,0], [0])
    ]
    
    stopRunning = 0

    def __init__(self):
        self.win = Tk()
        self.win.title("Nebula Python demos")
        self.win.geometry("270x100+580+1")
        self.win.resizable(0, 0)
        self.win.protocol('WM_DELETE_WINDOW', self.destroy)

        self.demo = IntVar()
        self.currentDemo = StringVar()
        self.currentDemo.set(self.scripts[0].name)
        self.currentDemoIndex = 0
        self.GFXDevice = StringVar()
        self.GFXDevice.set("GL")
        
        self.mainFrame = Frame(self.win, relief=SUNKEN, borderwidth=1)
        self.mainFrame.pack(side=TOP, expand=0)
        
        self.gfxButton = Menubutton(self.mainFrame,
            width=213, text=self.GFXDevice, underline=0, indicatoron=1,
            relief=RAISED, textvariable=self.GFXDevice
        )
        self.gfxButton.pack(side=TOP, fill=X)
        menu = Menu(self.gfxButton, tearoff=0)
        menu.add_radiobutton(label=" Direct3D V8 ", command=self.GFXSetDX)
        menu.add_radiobutton(label=" OpenGL ", command=self.GFXSetOGL)
        self.gfxButton.menu = self.gfxButton['menu'] = menu
        
        self.sourceViewer = SourceView(self.demo, self.scripts)
        
        b = Button(self.mainFrame, height=1, text="Current Source", command=self.sourceViewer.viewSource)
        b.pack(side=RIGHT, fill=X, expand=1)
        b = Button(self.mainFrame, height=1, text="Toggle Console", command=self.toggleConsole)
        b.pack(side=RIGHT, fill=X, expand=1)
        
        self.demoFrame = Frame(self.win, bd=2, relief=SUNKEN)
        self.demoFrame.pack(expand=YES, fill=BOTH)
        self.demoFrame.label = Label(self.demoFrame, text="Select Demo:")
        self.demoFrame.label.pack(side=LEFT, expand=NO, fill=X)
        
        self.demoMenu = Menubutton(self.demoFrame, width=213, underline=0,
            indicatoron=1, relief=RAISED, textvariable=self.currentDemo
        )
        self.demoMenu.pack(side=TOP, fill=X)
        menu = Menu(self.demoMenu, tearoff=0)
        for i, e in zip(range(len(self.scripts)), self.scripts):
            menu.add_radiobutton(label=e.name, command=self.menuCall, variable=self.demo, value=i)
        self.demoMenu['menu'] = self.demoMenu.menu = menu
        
        self.execNamespace = {
            'sel': npython.sel, 'new': npython.new, 'delete': npython.delete,
            'set': npython.set, 'psel': npython.psel, 'exists': npython.exists
        }


    def menuCall(self):
        vars = ('lookat.txyz', 'lookat.rxyz', 'camera.tz')
        s = self.scripts[self.demo.get()]
        self.currentDemo.set(s.name)
        try:
            self.loadScript(s.file, s.brightness)
            for (var, val) in zip(vars, s.args):
                if val:
                    npython.set('/usr/%s' % (var,), *val)
        except:
            traceback.print_exc()


    def toggleConsole(self):
        npython.set('/sys/servers/console.toggle')


    def loadScript(self, script, brightness):
        npython.sel('/')
        npython.delete('/usr/scene')
        npython.new('n3dnode', '/usr/scene')
        npython.set('/sys/servers/console.unwatch')
        npython.set('/sys/servers/time.starttime')
        npython.set('/sys/servers/shadow.setcastshadows', 0)
        
        if self.execNamespace.has_key('tl'):
            self.execNamespace['tl'].destroy()
            del self.execNamespace['tl']
        
        npython.set('/sys/servers/time.lockdeltat', 0.0)
        
        gfx = npython.sel('/sys/servers/gfx')
        apply(gfx.setclearcolor, ((brightness),) * 4)

        self.execNamespace['bg'] = brightness
        print 'Executing', script
        execfile(script, self.execNamespace)
        observerlib.orig()


    def GFXSetOGL(self):
        self.GFXDevice.set('GL')
        observerlib.togl()

    def GFXSetDX(self):
        self.GFXDevice.set('DX8')
        observerlib.tod3d8()
    
    
    def update(self):
        self.win.update()
    
    
    def destroy(self):
        self.stopRunning = 1



def main():
    demo = TekDemo()
    observerlib.init_observer()
    npython.set('/observer.setsleep', 0.02)
    npython.set('/observer.setgrid', 0)
    npython.set('/observer.startsinglestep')
    demo.menuCall()

    try:
        while not demo.stopRunning:
            npython.set('/observer.rendersinglestep')
            demo.update()
    except KeyboardInterrupt:
        pass

if __name__ == '__main__':
    main()
