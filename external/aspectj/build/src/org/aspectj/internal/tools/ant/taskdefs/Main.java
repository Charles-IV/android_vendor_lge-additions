package org.aspectj.internal.tools.ant.taskdefs;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.Date;
import java.util.TimeZone;
import java.text.SimpleDateFormat;

import org.apache.tools.ant.DefaultLogger;
import org.apache.tools.ant.listener.AnsiColorLogger;
import org.apache.tools.ant.taskdefs.Copy;
import org.apache.tools.ant.Project;

import org.aspectj.internal.tools.ant.taskdefs.BuildModule;
import org.aspectj.internal.tools.build.BuildSpec;

public class Main
{
    public File   baseDir;
    public File   jarDir;
    public File   tempDir;
    public File   productDir;
    public File   distDir;
    public String versionShort;
    public String versionBase;
    public String version;
    public String versionLong;

    public Main() {
        try {
            baseDir    = Options.baseDir;
            jarDir     = new File(Options.outDir.getCanonicalPath(), "jars");
            jarDir.mkdirs();
            tempDir    = new File(Options.outDir.getCanonicalPath(), "temp");
            tempDir.mkdirs();
            distDir    = new File(Options.outDir.getCanonicalPath(), "dist");
            distDir.mkdirs();
            productDir = new File(Options.baseDir.getCanonicalPath(), "build/products/tools");
            versionLong= "";
        } catch (Exception e) {
            handleException(e);
        }
    }

    private void readLocalProperties() {
        try {
            BufferedReader in = new BufferedReader(new FileReader(this.baseDir + "/build/local.properties"));
            String s;

            while ((s = in.readLine()) != null) {
                // System.out.println("line : "+s);
                if (s.contains("build.version.base=")) {
                    this.versionBase = s.split("=")[1];
                } else if (s.contains("build.version.long=")) {
                    this.versionLong = s.split("=")[1];
                } else if (s.contains("build.version.short=")) {
                    this.versionShort = s.split("=")[1];
                } else if (s.contains("build.version=")) {
                    this.version = s.split("=")[1];
                }
            }

            in.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void initProperties(Project project) {
        if (Options.verbose) {
            project.setProperty("verbose", "true");
        }
        project.setName("build");

        AnsiColorLogger dLogger = new AnsiColorLogger();
        dLogger.setMessageOutputLevel(Project.MSG_INFO);
        dLogger.setOutputPrintStream(System.out);
        dLogger.setErrorPrintStream(System.err);
        project.addBuildListener(dLogger);

        project.addFilter("company.name", "aspectj.org");
        project.addFilter("build.version.short", this.versionShort);
        project.addFilter("build.version.long", this.versionLong);
        project.addFilter("build.version.base", this.versionBase);
        project.addFilter("build.version", this.version);

        Date dNow = new Date();
        String dateFormat = "EEEE MMM d, yyyy 'at' HH:mm:ss z";
        TimeZone tz = TimeZone.getTimeZone("GMT");
        SimpleDateFormat ft = new SimpleDateFormat(dateFormat);
        ft.setTimeZone(tz);
        String nowTime = ft.format(dNow);

        project.addFilter("build.time", nowTime);
        project.addFilter("build.time.format", dateFormat);
    }

    private void initVersion(Project project) {
        Copy copy = new Copy();
        copy.setProject(project);
        copy.setTaskName("ajbuild-" + "copy");
        copy.setOverwrite(true);
        copy.setFiltering(false);

        copy.setFile(new File(this.baseDir, "bridge/src/org/aspectj/bridge/Version.java"));
        copy.setTofile(new File(this.tempDir, "BridgeVersion.java"));
        copy.execute();

        copy.setFiltering(true);

        copy.setFile(new File(this.baseDir, "build/lib/BridgeVersion.java.txt"));
        copy.setTofile(new File(this.baseDir, "bridge/src/org/aspectj/bridge/Version.java"));
        copy.execute();
    }

    private void restoreBridgeVersion(Project project) {
        Copy copy = new Copy();
        copy.setProject(project);
        copy.setTaskName("ajbuild-" + "copy");
        copy.setOverwrite(true);
        copy.setFiltering(false);

        copy.setFile(new File(this.tempDir, "BridgeVersion.java"));
        copy.setTofile(new File(this.baseDir, "bridge/src/org/aspectj/bridge/Version.java"));
        copy.execute();
    }


    private void initBuildSpec(BuildSpec buildSpec) {
        buildSpec.moduleDir       = null;
        buildSpec.module          = null;
        buildSpec.version         = this.versionLong;

        buildSpec.baseDir         = this.baseDir;
        buildSpec.jarDir          = this.jarDir;

        buildSpec.trimTesting     = true;
        buildSpec.assembleAll     = false;

        buildSpec.rebuild         = false;
        buildSpec.failonerror     = true;
        buildSpec.createInstaller = false;
        buildSpec.verbose         = true;

        buildSpec.productDir      = this.productDir;
        buildSpec.tempDir         = this.tempDir;
        buildSpec.distDir         = this.distDir;
    }

    public static void main(String[] args) {
        try {
            Options.loadArgs(args);

            Main main               = new Main();

            Project project         = new Project();
            BuildModule buildModule = new BuildModule();
            BuildSpec buildSpec     = new BuildSpec();

            main.readLocalProperties();
            main.initProperties(project);
            main.initVersion(project);
            main.initBuildSpec(buildSpec);

            buildModule.setProject(project);

            buildModule.buildSpec = buildSpec;
            buildModule.execute();

            main.restoreBridgeVersion(project);
        } catch (Exception e) {
            handleException(e);
        }
    }

    public static void handleException(Throwable e) {
        System.out.println("internal error: " + e.toString());
        e.printStackTrace();
        System.exit(-1);
    }
}

class Options {
    public static boolean verbose = false;
    public static File baseDir = null;
    public static File outDir  = null;

    public static void loadArgs(String[] args) {
        if (args == null) {
            return;
        }
        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (arg == null) {
                continue;
            }

            if (arg.equals("-verbose")) {
                verbose = true;
            } else if (arg.equals("-base")) {
                String next = "no argument";
                if (i + 1 < args.length) {
                    next = args[++i];
                    baseDir = new File(next);
                }
            } else if (arg.equals("-out")) {
                String next = "no argument";
                if (i + 1 < args.length) {
                    next = args[++i];
                    outDir = new File(next);
                }
            }
        }
    }
}
